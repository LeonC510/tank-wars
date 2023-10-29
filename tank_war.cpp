// Std. Includes
#include<iostream>
#include<map>
#include<string>
#include<cmath>
#include<vector>
#include<fstream>
#include<time.h>
//glfw
#include<glad/glad.h>
#include<GLFW/glfw3.h>
//freetype
#include <ft2build.h>
#include FT_FREETYPE_H
//glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//自定义头文件
#include"structs.h"
#include"shaderSources.h"
#include"otherDefines.h"
//#include"paths.h"

using namespace std;

enum colours { CLR_RED, CLR_BLUE, CLR_WITE, CLR_GREA, CLR_BLACK, CLR_YELLOW };	//定义颜色枚举（红，蓝，白，灰，黑，黄）
enum sides { SIDE_ENEMY, SIDE_SELF };										//定义坦克立场枚举（己方，敌方）
enum direction { DIR_FORWARD, DIR_RIGHT, DIR_BACK, DIR_LEFT };				//定义方向枚举（前，后，左，右）
enum tank_state { TS_VISIBLE, TS_INVISIBLE };								//定义坦克状态枚举（可见，不可见）
enum game_screen { GS_PLAY, GS_LOST, GS_WIN, GS_HOME, GS_INS};						//定义游戏页面枚举（玩，输，赢，主页）

//炮弹类
class Cmissile
{
private:
	int radius = 7;			//半径
	int breach_side = 3;	//缺口边长

	void draw_missile(position pos);	//炮弹绘制方法

	int speed;							//速度

public:
	enum direction direction_now;		//当前方向
	position position_now;				//当前坐标

	void move();		//炮弹移动方法
	void draw();						//炮弹绘制方法
	Cmissile(int pos_x, int pos_y, enum direction dir, int missile_speed);	//构造函数
	bool hit_test();
	int get_speed();

protected:

};

//坦克类
class Ctank
{
private:
	int body_width = 45, body_height = 80, breach_side = 11;			//坦克主体宽，高，缺口边长
	int track_width = 13, track_height = body_height, track_num = 9;	//坦克履带宽，高，节数
	int snout_width = 9, snout_lenth = 44;								//坦克炮管宽，长
	int snout_setback = 10;			//发弹时，炮管回缩度
	int missile_interval = 500;

	void draw_tank(int pos_x, int pos_y, enum sides Side, bool Track_status, enum direction dir, bool normal_or_black);		//绘制坦克方法
	void draw_tank_body(int x, int y, int width, int height, enum direction dir, enum colours colour);		//绘制坦克主体函数
	void draw_tank_track(int x, int y, int width, int height, int tank_body_width, int tank_body_height, int num, bool Track_status, direction dir, bool normal_or_black);	//绘制坦克履带函数
	void draw_tank_snout(int x, int y, int width, int lenth, int tank_body_height, direction dir, enum colours colour); 	//绘制炮管函数

	enum direction direction_now;	//当前方向
	position position_now;			//当前坐标
	enum tank_state state = TS_VISIBLE;	//坦克状态

	bool track_status = 0;			//履带状况（用于实现履带滚动效果）
	int speed_normal;						//坦克速度
	int speed_fine;					//坦克细调速度
	int current_moving_speed;		//当前坦克速度
	int missile_speed;				//炮弹速度
	enum sides side;				//坦克立场
	bool moving = false;
	string path;
	int path_pointer = 0;

	void start_moving(int move_speed);						//开始移动方法
	void stop_moving();										//停止移动方法
	void move_draw(enum direction dir_now, int unit_num);	//移动绘制方法
	void turn(direction dir);								//转向方法
	bool cover_test();

	friend void win_or_lost_test();
	friend void reset();
	friend void play_screen();

public:
	void draw();	//绘制方法
	void move();	//移动方法
	void process_input(GLFWwindow* window, int key, int scancode, int action, int mods);	//输入加工
	Ctank(int x_pos, int y_pos, enum direction tank_direction, int tank_speed, int tank_speed_fine, enum sides tank_side, int tank_missile_speed, string tank_path);		//构造函数

	vector <Cmissile> missiles;
	void missile_move();
	void missile_hit_test();
	void fire();
	bool position_test(position pos);
	void hit();

protected:

};

class Cbrick
{
private:
	int width = 100, height = 100;		//砖块宽，高

	void draw_brick(int x_pos, int y_pos);
	friend int init();
	friend void reset();

public:
	position brick_position;
	void draw();
	Cbrick(int pos_x, int pos_y);
	bool position_test(position pos);

protected:
};

class Cfind_in_ini
{
private:
	string file;
	vector<int> level_starting_points;
	vector<int> level_ending_points;
	int level_num;

public:
	Cfind_in_ini(string ini_file);
	vector<string> find_paths(int level);
	vector< vector<bool> > find_map(int level);

protected:
};

int screen_width = 800, screen_height = 600;	//屏幕宽高
unsigned int VBO_rec, VBO_tri, VAO_rec, VAO_tri, EBO_rec;	//矩形、三角形的VBO、VAO、EBO定义
GLuint VAO_text, VBO_text;
unsigned int shaderProgram_red, shaderProgram_blue, shaderProgram_wite, shaderProgram_grea, shaderProgram_black, shaderProgram_yellow, shaderProgram_text;		//着色器程序声明
GLFWwindow* window;		//窗口对象声明
map<GLchar, Character> Characters;
bool to_time = false;
fstream file_reader;

position tank_self_original_position{ 1000, 50 };

Ctank tank_self(tank_self_original_position.x, tank_self_original_position.y, DIR_FORWARD, 60, 20, SIDE_SELF, 30, "");	//己方坦克声明
vector <Ctank> enemy_tanks;		//敌方坦克容器声明
vector <Cbrick> bricks;
vector <string> enemy_tank_paths;
vector < vector<bool> > brick_map;
bool enemy_tanks_filled = false;
enum game_screen screen = GS_HOME;
int rounds = 0;

int init();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);			//窗口大小变化回调函数声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);	//按键回调函数声明
rec_data rec_data_convert(int x_pos, int y_pos, int width, int height);				//矩形数据转换函数声明
position_float pos_convert(int x_pos, int y_pos);									//坐标转换函数声明
position_float text_pos_convert(int x_pos, int y_pos);
void draw_rec(int x_pos, int y_pos, int width, int height, enum colours colour);	//矩形绘制函数声明
void draw_tri(int x_pos_a, int y_pos_a, int x_pos_b, int y_pos_b, int x_pos_c, int y_pos_c, enum colours colour);	//三角形绘制函数声明
void draw_text(string text, int x, int y, GLfloat scale, glm::vec3 color);
bool out_screen_test(position pos);
void reset();
void control();		//程序控制函数
void win_or_lost_test();
void play_screen();		//玩界面
void win_screen();		//胜利界面
void lost_screen();		//输界面
void home_screen();		//主页
void instructions_screen();

int main()
{
	if (init() != 0)
	{
		return -1;
	}

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	double start, stop, durationTime;

	while (!glfwWindowShouldClose(window))		//主循环
	{
		glfwPollEvents();	//检查触发事件

		if (to_time == true)
		{
			start = clock();
		}

		control();

		if (to_time == true)
		{
			stop = clock();
			durationTime = ((double)(stop - start)) / CLK_TCK;
			cout << "rander time spend: " << durationTime << " s" << endl;
			to_time = false;
		}
		
		glfwSwapBuffers(window);	//交换缓冲
	}

	glDeleteBuffers(1, &VBO_rec);	//删除矩形顶点缓冲对象
	glDeleteBuffers(1, &EBO_rec);	//删除矩形顶点索引对象
	glDeleteBuffers(1, &VBO_tri);	//删除三角形顶点缓冲对象
	glDeleteProgram(shaderProgram_red);		//删除着色器程序
	glDeleteProgram(shaderProgram_blue);	//同上
	glDeleteProgram(shaderProgram_wite);	//同上
	glDeleteProgram(shaderProgram_grea);	//同上
	glDeleteProgram(shaderProgram_black);	//同上
	glfwTerminate();	//释放资源
	return 0;	//退出程序
}

void control()
{
	switch (screen)
	{
	case GS_PLAY:
		win_or_lost_test();
		play_screen();
		break;

	case GS_LOST:
		lost_screen();
		break;

	case GS_WIN:
		win_screen();
		break;

	case GS_HOME:
		home_screen();
		break;

	case GS_INS:
		instructions_screen();
		break;

	default:
		home_screen();
		break;
	}
}

void play_screen()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	tank_self.draw();			//绘制坦克
	tank_self.missile_move();	//移动炮弹
	tank_self.missile_hit_test();	//炮弹击中测试
	tank_self.move();

	for (unsigned int i = 0; i < enemy_tanks.size(); i++)
	{
		enemy_tanks[i].draw();
	}

	for (unsigned int i = 0; i < enemy_tanks.size(); i++)
	{
		enemy_tanks[i].missile_move();
	}

	for (unsigned int i = 0; i < enemy_tanks.size(); i++)
	{
		enemy_tanks[i].missile_hit_test();
	}

	for (unsigned int i = 0; i < enemy_tanks.size(); i++)
	{
		enemy_tanks[i].move();
	}

	for (unsigned int i = 0; i < bricks.size(); i++)
	{
		bricks[i].draw();
	}
}

void home_screen()
{
	glClearColor(0.01568627450980392156862745098039f, 0.07058823529411764705882352941176f, 0.43529411764705882352941176470588f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	draw_text("Press the numbers on your keybord to acceess that level,", 100, 1800, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f));
	draw_text("or press 'i' to see introductions.", 100, 1740, 0.35f, glm::vec3(1.0f, 1.0f, 1.0f));

	for (int j = 0; j < 3; j++)
	{
		for (int i = 1; i <= 6; i++)
		{
			draw_text(to_string(i + j * 6), 200 + 300 * (i - 1), 1400 - 300 * j, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		}
	}
}

void instructions_screen()
{
	glClearColor(0.01568627450980392156862745098039f, 0.07058823529411764705882352941176f, 0.43529411764705882352941176470588f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	draw_text("INSTRUCTIONS", 730, 1750, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
	draw_text("Press the arrows on your keybord to move around,", 100, 1500, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
	draw_text("and press 'space' key to fire.", 100, 1300, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
	draw_text("Try to hit all the tanks to win! If you get hit,", 100, 1000, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
	draw_text("though, you'll lose.", 100, 800, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
	draw_text("Press 'Home' key at any time to go to home screen,", 100, 500, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
	draw_text("or press 'Esc' key to exit the game.", 100, 300, 0.6f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void win_screen()
{
	glClearColor(0.8509803921568627f, 0.8509803921568627f, 0.0980392156862745f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	draw_text("Congratulations", 575, 1600, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	draw_text("You win!", 550, 900, 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	draw_text("Press 'Home' key to get back to home screen.", 865, 150, 0.45f, glm::vec3(1.0f, 0.0f, 0.0f));
}

void lost_screen()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	draw_text("oops!", 800, 1400, 1.5f, glm::vec3(0.3f, 1.0f, 0.3f));
	draw_text("You lost", 600, 900, 2.0f, glm::vec3(0.3f, 1.0f, 0.3f));
	draw_text("Press 'Home' key to get back to home screen.", 865, 150, 0.45f, glm::vec3(0.3f, 1.0f, 0.3f));
}

void win_or_lost_test()
{
	if (enemy_tanks.size() <= 0 && enemy_tanks_filled == true)
	{
		screen = GS_WIN;
		rounds++;
	}
	else if (tank_self.state == TS_INVISIBLE)
	{
		screen = GS_LOST;
		rounds++;
	}
	else
	{
		screen = GS_PLAY;
	}
}

void reset()
{
	enemy_tanks_filled = false;

	tank_self.position_now = tank_self_original_position;
	tank_self.direction_now = DIR_FORWARD;
	tank_self.state = TS_VISIBLE;
	tank_self.current_moving_speed = tank_self.speed_fine;
	tank_self.moving = false;

	string setting_file;
	file_reader.open("tankWarConf.ini", ios::in);
	for (string reader; reader != "[file_end]"; file_reader >> reader)
	{
		if (reader != "")
		{
			setting_file.append(reader);
			setting_file.append("\r\n");
		}
	}
	file_reader.close();

	Cfind_in_ini ini_finder(setting_file);
	enemy_tank_paths = ini_finder.find_paths(1);
	brick_map = ini_finder.find_map(1);

	enemy_tanks_filled = false;
	enemy_tanks.clear();
	for (int i = 0; i < 13; i++)			//创建敌方坦克
	{
		enemy_tanks.push_back(Ctank(enemy_tanks_original_positions[i].x, enemy_tanks_original_positions[i].y, DIR_BACK, 30, 20, SIDE_ENEMY, 20, enemy_tank_paths[i]));
	}
	enemy_tanks_filled = true;

	bricks.clear();
	vector <position> brick_side;
	for (int y = 0; y < 19; y++)
	{
		for (int x = 0; x < 19; x++)
		{
			if (brick_map[y][x] == true)
			{
				bricks.push_back(Cbrick(50 + x * (100 + 5), (2000 - y * (100 + 5)) - 50));

				/*brick_side.push_back({ bricks.back().brick_position.x + bricks.back().width, bricks.back().brick_position.y + bricks.back().height });
				brick_side.push_back({ bricks.back().brick_position.x - bricks.back().width, bricks.back().brick_position.y + bricks.back().height });
				brick_side.push_back({ bricks.back().brick_position.x + bricks.back().width, bricks.back().brick_position.y - bricks.back().height });
				brick_side.push_back({ bricks.back().brick_position.x - bricks.back().width, bricks.back().brick_position.y - bricks.back().height });
				brick_side.push_back({ bricks.back().brick_position.x, bricks.back().brick_position.y - bricks.back().height });
				brick_side.push_back({ bricks.back().brick_position.x, bricks.back().brick_position.y + bricks.back().height });
				brick_side.push_back({ bricks.back().brick_position.x + bricks.back().width, bricks.back().brick_position.y });
				brick_side.push_back({ bricks.back().brick_position.x - bricks.back().width, bricks.back().brick_position.y });
				for (int i = 0; i < brick_side.size(); i++)
				{
					if (tank_self.position_test(brick_side[i]))
					{
						bricks.pop_back();
					}

					for (int i = 0; i < enemy_tanks.size(); i++)
					{
						if (enemy_tanks[i].position_test(brick_side[i]))
						{
							bricks.pop_back();
						}
					}
				}*/
			}
		}
	}
}

bool Cbrick::position_test(position pos)
{
	return ((pos.x >= brick_position.x - width / 2) && (pos.x <= brick_position.x + width / 2) && (pos.y >= brick_position.y - height / 2) && (pos.y <= brick_position.y + height / 2));
}

Cbrick::Cbrick(int pos_x, int pos_y)
{
	brick_position.x = pos_x;
	brick_position.y = pos_y;
}

void Cbrick::draw()
{
	draw_brick(brick_position.x, brick_position.y);
}

void Cbrick::draw_brick(int x_pos, int y_pos)
{
	draw_rec(x_pos - width / 2, y_pos + height / 2, width, height, CLR_YELLOW);
}

/*
* 描述：键盘事件回调函数
* 功能：对键盘事件做相应的响应
* 参数：所发生的窗口，按键名称，按键代码，发生的动作，发生时CapsLock和NumLock的状态
*/
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	else if (key == GLFW_KEY_HOME && action == GLFW_PRESS && screen != GS_HOME)
	{
		screen = GS_HOME;
	}
	else
	{
		if (screen == GS_PLAY)
		{
			tank_self.process_input(window, key, scancode, action, mods);
			if (action == GLFW_PRESS && key == GLFW_KEY_T)
			{
				to_time = true;
			}
		}
		else if (screen == GS_HOME)
		{
			if (key >= 48 && key <= 57 && action == GLFW_PRESS)
			{
				if (rounds != 0)
				{
					reset();
				}
				screen = GS_PLAY;
			}
			else if (key == GLFW_KEY_I && action == GLFW_PRESS)
			{
				screen = GS_INS;
			}
		}
	}
}

vector<string> Cfind_in_ini::find_paths(int level)
{
	vector <string> paths;

	int paths_section_starting_point = file.find("[tank_paths]", level_starting_points[(short)level - 1]) + sizeof("[tank_paths]") + 1;
	int paths_section_ending_point = file.find("[end]", paths_section_starting_point) - sizeof("[end]");

	for (int i = paths_section_starting_point; i <= paths_section_ending_point; i = file.find_first_of("\r\n", i) + 2)
	{
		paths.push_back(file.substr(i, file.find_first_of("\r\n", i) - i));
	}

	return paths;
}

vector< vector<bool> > Cfind_in_ini::find_map(int level)
{
	vector< vector<bool> > map;

	int map_section_starting_point = file.find("[map]", level_starting_points[(short)level - 1]) + sizeof("[map]");
	int map_section_ending_point = file.find("[end]", map_section_starting_point) - 2;

	vector <bool> map_line;

	for (int i = map_section_starting_point; i < map_section_ending_point; i += 19)
	{
		for (int j = 19; j > 0; j--)
		{
			if (file.substr(i + 20 - j, 1) == "1")
			{
				map_line.push_back(true);
			}
			else
			{
				map_line.push_back(false);
			}
		}
		map.push_back(map_line);
		map_line.clear();
	}

	return map;
}

Cfind_in_ini::Cfind_in_ini(string ini_file)
{
	file = ini_file;

	for(int i = 0; ; i++)
	{
		if (!level_ending_points.empty())
		{
			level_starting_points.push_back(file.find_first_of("[level_" + (char)i + ']', level_ending_points[i]));
		}
		else
		{
			level_starting_points.push_back(file.find_first_of("[level_" + (char)i + ']'));
		}
		level_ending_points.push_back(file.find_first_of("[level_end]", level_starting_points[i]));

		if (level_starting_points[level_starting_points.size()] > (int)file.find("[file_end]"))		//如已没有更多的关卡，则退出
		{
			level_starting_points.pop_back();
			level_ending_points.pop_back();
			level_num = i;
			break;
		}
	}
}

bool Cmissile::hit_test()
{
	if (position_now.x <= 0 || position_now.x >= 2000 || position_now.y <= 0 || position_now.y >= 2000)
	{
		return true;
	}
	else
	{
		if (tank_self.position_test(position_now))
		{
			tank_self.hit();
			return true;
		}

		for (unsigned int i = 0; i < enemy_tanks.size(); i++)
		{
			if (enemy_tanks[i].position_test(position_now))
			{
				enemy_tanks.erase(enemy_tanks.begin() + i);
				return true;
			}
		}

		for (unsigned int i = 0; i < bricks.size(); i++)
		{
			if (bricks[i].position_test(position_now))
			{
				bricks.erase(bricks.begin() + i);
				return true;
			}
		}

		return false;
	}
}

int Cmissile::get_speed()
{
	return speed;
}

/*
* 描述：类‘Cmissile’的移动方法
* 功能：让炮弹移动
* 参数：移动方向
*/
void Cmissile::move()
{
	switch (direction_now)
	{
	case DIR_FORWARD:
		draw();
		position_now.y += speed;
		break;

	case DIR_BACK:
		draw();
		position_now.y -= speed;
		break;

	case DIR_LEFT:
		draw();
		position_now.x -= speed;
		break;

	case DIR_RIGHT:
		draw();
		position_now.x += speed;
		break;
	}
}

/*
* 描述：类‘missile’的构造函数
* 功能：设置炮弹属性的初始值
* 参数：x坐标，y坐标，方向，速度
*/
Cmissile::Cmissile(int pos_x, int pos_y, enum direction dir, int missile_speed)
{
	position_now.x = pos_x;
	position_now.y = pos_y;
	direction_now = dir;
	speed = missile_speed;
}

void Cmissile::draw()
{
	draw_missile(position_now);
}

void Cmissile::draw_missile(position pos)
{
	draw_rec(pos.x - (radius * screen_height / screen_width), pos.y + (radius * screen_width / screen_height) - breach_side, 2 * (radius * screen_height / screen_width), 2 * ((radius * screen_width / screen_height) - breach_side), CLR_WITE);
	draw_rec(pos.x - ((radius * screen_height / screen_width) - breach_side), pos.y + (radius * screen_width / screen_height), 2 * ((radius * screen_height / screen_width) - breach_side), 2 * (radius * screen_width / screen_height), CLR_WITE);
}

bool Ctank::cover_test()
{
	position upper_left, upper_right, snout_top;

	switch (direction_now)
	{
	case DIR_FORWARD:
		upper_left.x = position_now.x - body_width / 2 - track_width;
		upper_left.y = position_now.y + body_height / 2;
		upper_right.x = position_now.x + body_width / 2 + track_width;
		upper_right.y = position_now.y + body_height / 2;
		snout_top.x = position_now.x;
		snout_top.y = position_now.y + body_height / 2 + snout_lenth;
		break;

	case DIR_BACK:
		upper_left.x = position_now.x + body_width / 2 + track_width;
		upper_left.y = position_now.y - body_height / 2;
		upper_right.x = position_now.x - body_width / 2 - track_width;
		upper_right.y = position_now.y - body_height / 2;
		snout_top.x = position_now.x;
		snout_top.y = position_now.y - body_height / 2 - snout_lenth;
		break;

	case DIR_LEFT:
		upper_left.x = position_now.x - (body_height * screen_height / screen_width) / 2;
		upper_left.y = position_now.y - (body_width * screen_width / screen_height) / 2 - (track_width * screen_width / screen_height);
		upper_right.x = position_now.x - (body_height * screen_height / screen_width) / 2;
		upper_right.y = position_now.y + (body_width * screen_width / screen_height) / 2 + (track_width * screen_width / screen_height);
		snout_top.x = position_now.x - (body_height * screen_height / screen_width) / 2 - (snout_lenth * screen_height / screen_width);
		snout_top.y = position_now.y;
		break;

	case DIR_RIGHT:
		upper_left.x = position_now.x + (body_height * screen_height / screen_width) / 2;
		upper_left.y = position_now.y + (body_width * screen_width / screen_height) / 2 + (track_width * screen_width / screen_height);
		upper_right.x = position_now.x + (body_height * screen_height / screen_width) / 2;
		upper_right.y = position_now.y - (body_width * screen_width / screen_height) / 2 - (track_width * screen_width / screen_height);
		snout_top.x = position_now.x + (body_height * screen_height / screen_width) / 2 + (snout_lenth * screen_height / screen_width);
		snout_top.y = position_now.y;
		break;
	}

	for (unsigned int i = 0; i < bricks.size(); i++)
	{
		if (bricks[i].position_test(upper_left) || bricks[i].position_test(upper_right) || bricks[i].position_test(snout_top))
		{
			return true;
		}
	}

	if (out_screen_test(upper_left) || out_screen_test(upper_right) || out_screen_test(snout_top))
	{
		return true;
	}

	for (unsigned int i = 0; i < enemy_tanks.size(); i++)
	{
		if (enemy_tanks[i].position_test(upper_left) || enemy_tanks[i].position_test(upper_right) || enemy_tanks[i].position_test(snout_top))
		{
			return true;
		}
	}

	if (tank_self.position_test(upper_left) || tank_self.position_test(upper_right) || tank_self.position_test(snout_top))
	{
		return true;
	}

	return false;
}

void Ctank::fire()
{
	if (missiles.size() != 0)
	{
		switch (direction_now)
		{
		case DIR_FORWARD:
			if (missiles.back().position_now.y - missiles.back().get_speed() - missile_interval > position_now.y + body_height / 2 + snout_lenth || missiles.back().direction_now != DIR_FORWARD)
			{
				missiles.push_back(Cmissile(position_now.x, position_now.y + snout_lenth + body_height / 2, direction_now, missile_speed));
			}
			break;

		case DIR_BACK:
			if (missiles.back().position_now.y + missiles.back().get_speed() + missile_interval < position_now.y - body_height / 2 - snout_lenth || missiles.back().direction_now != DIR_BACK)
			{
				missiles.push_back(Cmissile(position_now.x, position_now.y - snout_lenth - body_height / 2, direction_now, missile_speed));
			}
			break;

		case DIR_LEFT:
			if (missiles.back().position_now.x + missiles.back().get_speed() + missile_interval < position_now.x - body_height / 2 - snout_lenth || missiles.back().direction_now != DIR_LEFT)
			{
				missiles.push_back(Cmissile(position_now.x - snout_lenth - body_height / 2, position_now.y, direction_now, missile_speed));
			}
			break;

		case DIR_RIGHT:
			if (missiles.back().position_now.x - missiles.back().get_speed() - missile_interval > position_now.x + body_height / 2 + snout_lenth || missiles.back().direction_now != DIR_RIGHT)
			{
				missiles.push_back(Cmissile(position_now.x + snout_lenth + body_height / 2, position_now.y, direction_now, missile_speed));
			}
			break;
		}
	}
	else
	{
		switch (direction_now)
		{
		case DIR_FORWARD:
			missiles.push_back(Cmissile(position_now.x, position_now.y + body_height / 2 + snout_lenth, direction_now, missile_speed));
			break;

		case DIR_BACK:
			missiles.push_back(Cmissile(position_now.x, position_now.y - body_height / 2 - snout_lenth, direction_now, missile_speed));
			break;

		case DIR_LEFT:
			missiles.push_back(Cmissile(position_now.x - body_height / 2 - snout_lenth, position_now.y, direction_now, missile_speed));
			break;

		case DIR_RIGHT:
			missiles.push_back(Cmissile(position_now.x + body_height / 2 + snout_lenth, position_now.y, direction_now, missile_speed));
			break;
		}
	}
}

void Ctank::missile_hit_test()
{
	for (unsigned int i = 0; i < missiles.size(); i++)
	{
		if (missiles[i].hit_test())
		{	//BUG! 在此点，missiles.size() 被清零
			if (missiles.empty() == false)
			{
				missiles.erase(missiles.begin() + i);
			}
		}
	}
}

void Ctank::missile_move()
{
	for (unsigned int i = 0; i < missiles.size(); i++)
	{
		missiles[i].move();
	}
}

void Ctank::hit()
{
	state = TS_INVISIBLE;
}

bool Ctank::position_test(position pos)
{
	if (direction_now == DIR_BACK || direction_now == DIR_FORWARD)
	{
		return ((pos.x > position_now.x - body_width / 2 - track_width) & (pos.x < position_now.x + body_width / 2 + track_width) & (pos.y > position_now.y - body_height / 2) & (pos.y < position_now.y + body_height / 2) & (state == TS_VISIBLE));
	}
	else
	{
		return ((pos.x > position_now.x - body_height * screen_height / screen_width / 2) & (pos.x < position_now.x + body_height * screen_height / screen_width / 2) & (pos.y > position_now.y - body_width * screen_width / screen_height / 2 - track_width * screen_width / screen_height) & (pos.y < position_now.y + (body_width * screen_width / screen_height) / 2 + (track_width * screen_width / screen_height)) & (state == TS_VISIBLE));
	}
}

void Ctank::process_input(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_UP:
			if (direction_now == DIR_FORWARD)
			{
				start_moving(speed_fine);
			}
			else
			{
				turn(DIR_FORWARD);
			}
			break;

		case GLFW_KEY_DOWN:
			if (direction_now == DIR_BACK)
			{
				start_moving(speed_fine);
			}
			else
			{
				turn(DIR_BACK);
			}
			break;

		case GLFW_KEY_LEFT:
			if (direction_now == DIR_LEFT)
			{
				start_moving(speed_fine);
			}
			else
			{
				turn(DIR_LEFT);
			}
			break;

		case GLFW_KEY_RIGHT:
			if (direction_now == DIR_RIGHT)
			{
				start_moving(speed_fine);
			}
			else
			{
				turn(DIR_RIGHT);
			}
			break;

		case GLFW_KEY_SPACE:
			tank_self.fire();
			break;

		default:
			draw();
			break;
		}
	}
	else if (action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_UP:
			if (direction_now == DIR_FORWARD)
			{
				start_moving(speed_normal);
			}
			else
			{
				turn(DIR_FORWARD);
			}
			break;

		case GLFW_KEY_DOWN:
			if (direction_now == DIR_BACK)
			{
				start_moving(speed_normal);
			}
			else
			{
				turn(DIR_BACK);
			}
			break;

		case GLFW_KEY_LEFT:
			if (direction_now == DIR_LEFT)
			{
				start_moving(speed_normal);
			}
			else
			{
				turn(DIR_LEFT);
			}
			break;

		case GLFW_KEY_RIGHT:
			if (direction_now == DIR_RIGHT)
			{
				start_moving(speed_normal);
			}
			else
			{
				turn(DIR_RIGHT);
			}
			break;

		case GLFW_KEY_SPACE:
			tank_self.fire();
			break;

		default:
			draw();
			break;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN || key == GLFW_KEY_RIGHT || key == GLFW_KEY_LEFT)
		{
			stop_moving();
		}
	}
}

Ctank::Ctank(int x_pos, int y_pos, enum direction tank_direction, int tank_speed, int tank_speed_fine, enum sides tank_side, int tank_missile_speed, string tank_path)
{
	position_now.x = x_pos;
	position_now.y = y_pos;
	direction_now = tank_direction;
	speed_normal = tank_speed;
	speed_fine = tank_speed_fine;
	side = tank_side;
	missile_speed = tank_missile_speed;
	path = tank_path;
	current_moving_speed = speed_fine;		//暂时设定当前速度为微调速度
}

void Ctank::turn(direction dir)
{
	direction dir_old = direction_now;

	draw_tank(position_now.x, position_now.y, side, track_status, dir_old, false);
	draw_tank(position_now.x, position_now.y, side, track_status, dir, true);
	direction_now = dir;
	stop_moving();

	if (cover_test())
	{
		draw_tank(position_now.x, position_now.y, side, track_status, dir, false);
		draw_tank(position_now.x, position_now.y, side, track_status, dir_old, true);
		direction_now = dir_old;
	}
}

void Ctank::start_moving(int move_speed)
{
	moving = true;
	current_moving_speed = move_speed;
}

void Ctank::stop_moving()
{
	moving = false;
}

void Ctank::move()
{
	if (moving == true)
	{
		move_draw(direction_now, current_moving_speed);
	}
	else if (path != "")
	{
		switch (path[path_pointer])
		{
			case 'b':
				if (direction_now != DIR_BACK)
				{
					turn(DIR_BACK);
				}
				start_moving(speed_normal);
				break;

			case 'f':
				if (direction_now != DIR_FORWARD)
				{
					turn(DIR_FORWARD);
				}
				start_moving(speed_normal);
				break;

			case 'l':
				if (direction_now != DIR_LEFT)
				{
					turn(DIR_LEFT);
				}
				start_moving(speed_normal);
				break;

			case 'r':
				if (direction_now != DIR_RIGHT)
				{
					turn(DIR_RIGHT);
				}
				start_moving(speed_normal);
				break;

			case 'B':
				if (direction_now != DIR_BACK)
				{
					turn(DIR_BACK);
				}
				start_moving(speed_fine);
				break;

			case 'F':
				if (direction_now != DIR_FORWARD)
				{
					turn(DIR_FORWARD);
				}
				start_moving(speed_fine);
				break;

			case 'L':
				if (direction_now != DIR_LEFT)
				{
					turn(DIR_LEFT);
				}
				start_moving(speed_fine);
				break;

			case 'R':
				if (direction_now != DIR_RIGHT)
				{
					turn(DIR_RIGHT);
				}
				start_moving(speed_fine);
				break;

			case 's':
				fire();
				break;

			default:
				break;
		}

		path_pointer++;
		if (path_pointer >= path.size())
		{
			path_pointer = 0;
		}
	}
}

void Ctank::move_draw(enum direction dir_now, int unit_num)
{
	static double j = 0;
	int track_change_speed = 20;
	static int count = 0;

	switch (dir_now)
	{
	case DIR_FORWARD:
		if (!cover_test())
		{
			position_now.y += 3;
		}
		draw_tank(position_now.x, position_now.y - 2, side, (int)floor(j) % 2, direction_now, false);
		draw_tank(position_now.x, position_now.y, side, (int)floor(j) % 2, direction_now, true);
		j += (double)1 / (double)track_change_speed;
		break;

	case DIR_BACK:
		if (!cover_test())
		{
			position_now.y -= 3;
		}
		draw_tank(position_now.x, position_now.y + 2, side, (int)floor(j) % 2, direction_now, false);
		draw_tank(position_now.x, position_now.y, side, (int)floor(j) % 2, direction_now, true);
		j += (double)1 / (double)track_change_speed;
		break;

	case DIR_LEFT:
		if (!cover_test())
		{
			position_now.x -= 3;
		}
		draw_tank(position_now.x + 2, position_now.y, side, (int)floor(j) % 2, direction_now, false);
		draw_tank(position_now.x, position_now.y, side, (int)floor(j) % 2, direction_now, true);
		j += (double)1 / (double)track_change_speed;
		break;

	case DIR_RIGHT:
		if (!cover_test())
		{
			position_now.x += 3;
		}
		draw_tank(position_now.x - 2, position_now.y, side, (int)floor(j) % 2, direction_now, false);
		draw_tank(position_now.x, position_now.y, side, (int)floor(j) % 2, direction_now, true);
		j += (double)1 / (double)track_change_speed;
		break;
	}

	if (count >= unit_num)
	{
		moving = false;
		track_status = j;
		count = 0;
	}

	count++;
}

void Ctank::draw()
{
	if (state == TS_VISIBLE)
	{
		draw_tank(position_now.x, position_now.y, side, track_status, direction_now, true);
	}
}

void Ctank::draw_tank(int pos_x, int pos_y, enum sides Side, bool Track_status, enum direction dir, bool normal_or_black)
{
	if (dir == DIR_FORWARD || dir == DIR_BACK)
	{
		if (normal_or_black == true)
		{
			if (Side == SIDE_ENEMY)
			{
				draw_tank_body(pos_x, pos_y, body_width, body_height, dir, CLR_BLUE);
			}
			else
			{
				draw_tank_body(pos_x, pos_y, body_width, body_height, dir, CLR_RED);
			}

			draw_tank_snout(pos_x, pos_y, snout_width, snout_lenth, body_height, dir, CLR_GREA);
		}
		else
		{
			draw_tank_body(pos_x, pos_y, body_width, body_height, dir, CLR_BLACK);
			draw_tank_snout(pos_x, pos_y, snout_width, snout_lenth, body_height, dir, CLR_BLACK);
		}
		draw_tank_track(pos_x, pos_y, track_width, track_height, body_width, body_height, track_num, Track_status, dir, normal_or_black);
	}
	else
	{
		if (normal_or_black == true)
		{
			if (Side == SIDE_ENEMY)
			{
				draw_tank_body(pos_x, pos_y, body_width * screen_width / screen_height, body_height * screen_height / screen_width, dir, CLR_BLUE);
			}
			else
			{
				draw_tank_body(pos_x, pos_y, body_width * screen_width / screen_height, body_height * screen_height / screen_width, dir, CLR_RED);
			}

			draw_tank_snout(pos_x, pos_y, snout_width * screen_width / screen_height, snout_lenth * screen_height / screen_width, body_height * screen_height / screen_width, dir, CLR_GREA);
		}
		else
		{
			draw_tank_body(pos_x, pos_y, body_width * screen_width / screen_height, body_height * screen_height / screen_width, dir, CLR_BLACK);
			draw_tank_snout(pos_x, pos_y, snout_width * screen_width / screen_height, snout_lenth * screen_height / screen_width, body_height * screen_height / screen_width, dir, CLR_BLACK);
		}
		draw_tank_track(pos_x, pos_y, track_width * screen_width / screen_height, track_height * screen_height / screen_width, body_width * screen_width / screen_height, body_height * screen_height / screen_width, track_num, Track_status, dir, normal_or_black);
	}
}

void Ctank::draw_tank_body(int x, int y, int width, int height, enum direction dir, enum colours colour)
{
	switch (dir)
	{
	case DIR_FORWARD:
		draw_rec(x - width / 2, y + height / 2 - breach_side, width, height - breach_side * 2, colour);
		draw_rec(x - width / 2 + breach_side, y + height / 2, width - breach_side * 2, height, colour);
		break;

	case DIR_BACK:
		draw_rec(x - width / 2, y + height / 2 - breach_side, width, height - breach_side * 2, colour);
		draw_rec(x - width / 2 + breach_side, y + height / 2, width - breach_side * 2, height, colour);
		break;

	case DIR_LEFT:
		draw_rec(x - height / 2 + breach_side, y + width / 2, height - breach_side * 2, width, colour);
		draw_rec(x - height / 2, y + width / 2 - breach_side, height, width - breach_side * 2, colour);
		break;

	case DIR_RIGHT:
		draw_rec(x - height / 2 + breach_side, y + width / 2, height - breach_side * 2, width, colour);
		draw_rec(x - height / 2, y + width / 2 - breach_side, height, width - breach_side * 2, colour);
		break;
	}
}

void Ctank::draw_tank_track(int x, int y, int width, int height, int tank_body_width, int tank_body_height, int num, bool Track_status, direction dir, bool normal_or_black)
{
	switch (dir)
	{
	case DIR_FORWARD:
		for (int i = 0; i < num; i++)
		{
			if (i % 2 + Track_status != 0 && i % 2 + Track_status < 2)
			{
				draw_rec(x - tank_body_width / 2 - width, y + tank_body_height / 2 - i * (height / num), track_width, height / num, normal_or_black ? CLR_GREA : CLR_BLACK);
				draw_rec(x + tank_body_width / 2, y + tank_body_height / 2 - i * (height / num), width, height / num, normal_or_black ? CLR_GREA : CLR_BLACK);
			}
			else
			{
				draw_rec(x - tank_body_width / 2 - width, y + tank_body_height / 2 - i * (height / num), width, height / num, normal_or_black ? CLR_WITE : CLR_BLACK);
				draw_rec(x + tank_body_width / 2, y + tank_body_height / 2 - i * (height / num), width, height / num, normal_or_black ? CLR_WITE : CLR_BLACK);
			}
		}
		break;

	case DIR_BACK:
		for (int i = 0; i < num; i++)
		{
			if (i % 2 + Track_status != 0 && i % 2 + Track_status < 2)
			{
				draw_rec(x - tank_body_width / 2 - width, y + tank_body_height / 2 - i * (height / num), track_width, height / num, normal_or_black ? CLR_GREA : CLR_BLACK);
				draw_rec(x + tank_body_width / 2, y + tank_body_height / 2 - i * (height / num), width, height / num, normal_or_black ? CLR_GREA : CLR_BLACK);
			}
			else
			{
				draw_rec(x - tank_body_width / 2 - width, y + tank_body_height / 2 - i * (height / num), width, height / num, normal_or_black ? CLR_WITE : CLR_BLACK);
				draw_rec(x + tank_body_width / 2, y + tank_body_height / 2 - i * (height / num), width, height / num, normal_or_black ? CLR_WITE : CLR_BLACK);
			}
		}
		break;

	case DIR_LEFT:
		for (int i = 0; i < num; i++)
		{
			if (i % 2 + Track_status != 0 && i % 2 + Track_status < 2)
			{
				draw_rec(x - tank_body_height / 2 + i * (height / num), y - tank_body_width / 2, height / num, width, normal_or_black ? CLR_GREA : CLR_BLACK);
				draw_rec(x - tank_body_height / 2 + i * (height / num), y + tank_body_width / 2 + width, height / num, width, normal_or_black ? CLR_GREA : CLR_BLACK);
			}
			else
			{
				draw_rec(x - tank_body_height / 2 + i * (height / num), y - tank_body_width / 2, height / num, width, normal_or_black ? CLR_WITE : CLR_BLACK);
				draw_rec(x - tank_body_height / 2 + i * (height / num), y + tank_body_width / 2 + width, height / num, width, normal_or_black ? CLR_WITE : CLR_BLACK);
			}
		}
		break;

	case DIR_RIGHT:
		for (int i = 0; i < num; i++)
		{
			if (i % 2 + Track_status != 0 && i % 2 + Track_status < 2)
			{
				draw_rec(x - tank_body_height / 2 + i * (height / num), y - tank_body_width / 2, height / num, width, normal_or_black ? CLR_GREA : CLR_BLACK);
				draw_rec(x - tank_body_height / 2 + i * (height / num), y + tank_body_width / 2 + width, height / num, width, normal_or_black ? CLR_GREA : CLR_BLACK);
			}
			else
			{
				draw_rec(x - tank_body_height / 2 + i * (height / num), y - tank_body_width / 2, height / num, width, normal_or_black ? CLR_WITE : CLR_BLACK);
				draw_rec(x - tank_body_height / 2 + i * (height / num), y + tank_body_width / 2 + width, height / num, width, normal_or_black ? CLR_WITE : CLR_BLACK);
			}
		}
		break;
	}
}

void Ctank::draw_tank_snout(int x, int y, int width, int lenth, int tank_body_height, direction dir, enum colours colour)
{
	switch (dir)
	{
	case DIR_FORWARD:
		draw_rec(x - width / 2, y + tank_body_height / 2 + lenth, width, lenth + tank_body_height / 2, colour);
		break;

	case DIR_BACK:
		draw_rec(x - width / 2, y, width, lenth + tank_body_height / 2, colour);
		break;

	case DIR_LEFT:
		draw_rec(x - tank_body_height / 2 - lenth, y + width / 2, lenth + tank_body_height / 2, width, colour);
		break;

	case DIR_RIGHT:
		draw_rec(x, y + width / 2, lenth + tank_body_height / 2, width, colour);
		break;
	}
}

bool out_screen_test(position pos)
{
	if (pos.x >= 2000 || pos.x <= 0 || pos.y >= 2000 || pos.y <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void draw_text(string text, int x, int y, GLfloat scale, glm::vec3 color)
{
	position_float pos = text_pos_convert(x, y);

	// 激活对应的渲染状态
	glUseProgram(shaderProgram_text);
	glUniform3f(glGetUniformLocation(shaderProgram_text, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO_text);

	// 遍历文本中所有的字符
	string::const_iterator i;
	for (i = text.begin(); i != text.end(); i++)
	{
		Character ch = Characters[*i];

		GLfloat xpos = pos.x + ch.Bearing.x * scale;
		GLfloat ypos = pos.y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// 对每个字符更新VBO
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// 在四边形上绘制字形纹理
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// 更新VBO内存的内容
		glBindBuffer(GL_ARRAY_BUFFER, VBO_text);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// 绘制四边形
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// 更新位置到下一个字形的原点，注意单位是1/64像素
		pos.x += (ch.Advance >> 6) * scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_tri(int x_pos_a, int y_pos_a, int x_pos_b, int y_pos_b, int x_pos_c, int y_pos_c, enum colours colour)
{
	position_float pos_a, pos_b, pos_c;

	pos_a = pos_convert(x_pos_a, y_pos_a);
	pos_b = pos_convert(x_pos_b, y_pos_b);
	pos_c = pos_convert(x_pos_c, y_pos_c);

	float vertices[] = {
		pos_a.x, pos_a.y, 0.0f,		//顶点1
		pos_b.x, pos_b.y, 0.0f,		//顶点2
		pos_c.x, pos_c.y, 0.0f,		//顶点3
	};

	glBindVertexArray(VAO_tri);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_tri);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);	//调用此函数须有一个VBO存在
	glEnableVertexAttribArray(0);

	switch (colour)
	{
	case CLR_RED:
		glUseProgram(shaderProgram_red);
		break;
	case CLR_BLUE:
		glUseProgram(shaderProgram_blue);
		break;
	case CLR_WITE:
		glUseProgram(shaderProgram_wite);
		break;
	case CLR_GREA:
		glUseProgram(shaderProgram_grea);
		break;
	case CLR_BLACK:
		glUseProgram(shaderProgram_black);
		break;
	case CLR_YELLOW:
		glUseProgram(shaderProgram_yellow);
		break;
	default:
		glUseProgram(shaderProgram_grea);
		break;
	}

	glBindVertexArray(VAO_tri);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void draw_rec(int x_pos, int y_pos, int width, int height, enum colours colour)
{
	rec_data data;

	data = rec_data_convert(x_pos, y_pos, width, height);

	float vertices[] = {
	data.x_pos, data.y_pos, 0.0f,							//左上角
	data.x_pos + data.width, data.y_pos, 0.0f,				//右上角
	data.x_pos, data.y_pos - data.height, 0.0f,				//左下角
	data.x_pos + data.width, data.y_pos - data.height, 0.0f,	//右下角
	};

	switch (colour)
	{
	case CLR_RED:
		glUseProgram(shaderProgram_red);
		break;
	case CLR_BLUE:
		glUseProgram(shaderProgram_blue);
		break;
	case CLR_WITE:
		glUseProgram(shaderProgram_wite);
		break;
	case CLR_GREA:
		glUseProgram(shaderProgram_grea);
		break;
	case CLR_BLACK:
		glUseProgram(shaderProgram_black);
		break;
	case CLR_YELLOW:
		glUseProgram(shaderProgram_yellow);
		break;
	default:
		glUseProgram(shaderProgram_grea);
		break;
	}

	glBindVertexArray(VAO_rec);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_rec);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_rec);
	glEnableVertexAttribArray(0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//glDrawArrays(GL_TRIANGLES, 0, 6);
}

position_float text_pos_convert(int x_pos, int y_pos)
{
	position_float fl_pos;
	int scale = 1000;

	fl_pos.x = ((float)x_pos / ((float)scale * 2)) * (float)screen_width;
	fl_pos.y = ((float)y_pos / ((float)scale * 2)) * (float)screen_height;

	return fl_pos;
}

rec_data rec_data_convert(int x_pos, int y_pos, int width, int height)
{
	rec_data fl_pos;
	int scale = 1000;

	fl_pos.x_pos = ((float)x_pos / (float)scale) - 1;
	fl_pos.y_pos = ((float)y_pos / (float)scale) - 1;
	fl_pos.width = ((float)width / (float)scale);
	fl_pos.height = ((float)height / (float)scale);

	return fl_pos;
}

position_float pos_convert(int x_pos, int y_pos)
{
	position_float fl_pos;
	int scale = 1000;

	fl_pos.x = ((float)x_pos / (float)scale) - 1;
	fl_pos.y = ((float)y_pos / (float)scale) - 1;

	return fl_pos;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screen_width = width;
	screen_height = height;
}

int init()
{
	if (!glfwInit())		//初始化GLFW，如失败，打印提示并退出
	{
		cout << "Failed to init GLFW" << endl;	//打印提示
		return -1;	//退出
	}
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);		//设置GLFW主版本号为3
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);		//设置GLFW次版本号为3

	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//设置GLFW为核心模式

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(screen_width, screen_height, "tank war", NULL, NULL);		//创建窗口

	if (window == NULL)		//检测窗口是否创建成功，如失败，打印提示并退出
	{
		cout << "Failed to create GLFW window" << endl;	//打印提示
		glfwTerminate();	//释放资源
		return -1;	//退出
	}

	glfwMakeContextCurrent(window);		//设置window为主上下文

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))		//加载GLAD，如失败，打印提示并退出
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);		//设置窗口大小变化回调函数为framebuffer_size_callback
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);					//启用粘滞键
	glfwSetKeyCallback(window, key_callback);								//设置按键回调为key_callback

	unsigned int vertexShader_graphic, vertexShader_text;		//声明顶点着色器变量
	vertexShader_graphic = glCreateShader(GL_VERTEX_SHADER);		//生成顶点着色器，存放在变量vertexShader_graphic
	vertexShader_text = glCreateShader(GL_VERTEX_SHADER);		//生成顶点着色器，存放在变量vertexShader
	glShaderSource(vertexShader_graphic, 1, &vertexShaderSource_graphic, NULL);		//设置着色器源码
	glShaderSource(vertexShader_text, 1, &vertexShaderSource_text, NULL);		//设置着色器源码
	glCompileShader(vertexShader_graphic);	//编译顶点着色器
	glCompileShader(vertexShader_text);	//编译顶点着色器

	unsigned int fragmentShader_red, fragmentShader_blue, fragmentShader_wite, fragmentShader_grea, fragmentShader_black, fragmentShader_yellow, fragmentShader_text;	//声明片段着色器变量
	fragmentShader_red = glCreateShader(GL_FRAGMENT_SHADER);	//生成顶点着色器，存放在变量fragmentShader_red
	fragmentShader_blue = glCreateShader(GL_FRAGMENT_SHADER);	//似上
	fragmentShader_wite = glCreateShader(GL_FRAGMENT_SHADER);	//似上
	fragmentShader_grea = glCreateShader(GL_FRAGMENT_SHADER);	//似上
	fragmentShader_black = glCreateShader(GL_FRAGMENT_SHADER);	//似上
	fragmentShader_yellow = glCreateShader(GL_FRAGMENT_SHADER);	//似上
	fragmentShader_text = glCreateShader(GL_FRAGMENT_SHADER);		//生成片段着色器，存放在变量fragmentShader
	glShaderSource(fragmentShader_red, 1, &fragmentShaderSource_red, NULL);		//设置着色器源码
	glCompileShader(fragmentShader_red);	//编译着色器
	glShaderSource(fragmentShader_blue, 1, &fragmentShaderSource_blue, NULL);	//设置着色器源码
	glCompileShader(fragmentShader_blue);	//编译着色器
	glShaderSource(fragmentShader_wite, 1, &fragmentShaderSource_wite, NULL);	//设置着色器源码
	glCompileShader(fragmentShader_wite);	//编译着色器
	glShaderSource(fragmentShader_grea, 1, &fragmentShaderSource_grea, NULL);	//设置着色器源码
	glCompileShader(fragmentShader_grea);	//编译着色器
	glShaderSource(fragmentShader_black, 1, &fragmentShaderSource_black, NULL);	//设置着色器源码
	glCompileShader(fragmentShader_black);	//编译着色器
	glShaderSource(fragmentShader_yellow, 1, &fragmentShaderSource_yellow, NULL);	//设置着色器源码
	glCompileShader(fragmentShader_yellow);	//编译着色器
	glShaderSource(fragmentShader_text, 1, &fragmentShaderSource, NULL);		//设置着色器源码
	glCompileShader(fragmentShader_text);	//编译片段着色器

	shaderProgram_red = glCreateProgram();		//创建红色着色器程序
	shaderProgram_blue = glCreateProgram();		//创建蓝色着色器程序
	shaderProgram_wite = glCreateProgram();		//创建白色着色器程序
	shaderProgram_grea = glCreateProgram();		//创建灰色着色器程序
	shaderProgram_black = glCreateProgram();	//创建黑色着色器程序
	shaderProgram_yellow = glCreateProgram();	//创建黄色着色器程序
	shaderProgram_text = glCreateProgram();		//创建文字着色器程序

	glAttachShader(shaderProgram_red, vertexShader_graphic);			//附着顶点着色器到红色着色器程序
	glAttachShader(shaderProgram_red, fragmentShader_red);		//附着片段着色器到红色着色器程序
	glLinkProgram(shaderProgram_red);	//链接着色器

	glAttachShader(shaderProgram_blue, vertexShader_graphic);			//附着顶点着色器到蓝色着色器程序
	glAttachShader(shaderProgram_blue, fragmentShader_blue);	//附着片段着色器到蓝色着色器程序
	glLinkProgram(shaderProgram_blue);	//链接着色器

	glAttachShader(shaderProgram_wite, vertexShader_graphic);			//附着顶点着色器到白色着色器程序
	glAttachShader(shaderProgram_wite, fragmentShader_wite);	//附着片段着色器到白色着色器程序
	glLinkProgram(shaderProgram_wite);	//链接着色器

	glAttachShader(shaderProgram_grea, vertexShader_graphic);			//附着顶点着色器到灰色着色器程序
	glAttachShader(shaderProgram_grea, fragmentShader_grea);	//附着片段着色器到灰色着色器程序
	glLinkProgram(shaderProgram_grea);	//链接着色器

	glAttachShader(shaderProgram_black, vertexShader_graphic);			//附着顶点着色器到黑色着色器程序
	glAttachShader(shaderProgram_black, fragmentShader_black);	//附着片段着色器到黑色着色器程序
	glLinkProgram(shaderProgram_black);	//链接着色器

	glAttachShader(shaderProgram_yellow, vertexShader_graphic);			//附着顶点着色器到黄色着色器程序
	glAttachShader(shaderProgram_yellow, fragmentShader_yellow);//附着片段着色器到黄色着色器程序
	glLinkProgram(shaderProgram_yellow);	//链接着色器

	glAttachShader(shaderProgram_text, vertexShader_text);			//附着顶点着色器到着色器程序
	glAttachShader(shaderProgram_text, fragmentShader_text);		//附着片段着色器到着色器程序
	glLinkProgram(shaderProgram_text);	//链接着色器

	glDeleteShader(vertexShader_graphic);	//删除图象顶点着色器（我们已不再需要它了）
	glDeleteShader(vertexShader_text);		//删除文字顶点着色器（我们已不再需要它了）
	glDeleteShader(fragmentShader_red);		//删除红色片段着色器（我们已不再需要它了）
	glDeleteShader(fragmentShader_blue);	//删除蓝色片段着色器（我们已不再需要它了）
	glDeleteShader(fragmentShader_wite);	//删除白色片段着色器（我们已不再需要它了）
	glDeleteShader(fragmentShader_grea);	//删除灰色片段着色器（我们已不再需要它了）
	glDeleteShader(fragmentShader_black);	//删除黑色片段着色器（我们已不再需要它了）
	glDeleteShader(fragmentShader_text);		//删除片段着色器（我们已不再需要它了）

	glm::mat4 projection = glm::ortho(0.0f, (float)screen_width, 0.0f, (float)screen_height);
	glUseProgram(shaderProgram_text);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram_text, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	FT_Library freetype;
	if (FT_Init_FreeType(&freetype))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	FT_Face face;
	if (FT_New_Face(freetype, "C:/windows/fonts/arial.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用字节对齐限制

	for (GLubyte i = 0; i < 128; i++)
	{
		// 加载字符的字形 
		if (FT_Load_Char(face, i, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// 生成纹理
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// 设置纹理选项
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// 储存字符供之后使用
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(i, character));
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(freetype);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &VAO_text);
	glGenBuffers(1, &VBO_text);
	glBindVertexArray(VAO_text);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_text);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenBuffers(1, &EBO_rec);			//生成矩形的EBO，存放在变量VBO_rec中
	glGenBuffers(1, &VBO_rec);			//生成矩形的VBO，存放在变量VBO_rec中
	glGenVertexArrays(1, &VAO_rec);		//生成矩形的VAO，存放在变量VAO_rec中
	glGenBuffers(1, &VBO_tri);			//生成三角形的VBO，存放在变量VBO_tri中
	glGenVertexArrays(1, &VAO_tri);		//生成三角形的VAO，存放在变量VAO_tri中

	glBindVertexArray(VAO_rec);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_rec);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 6, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);	//调用此函数须有一个VBO存在
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_rec);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rec_EBO_indices), rec_EBO_indices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	

	string setting_file;
	file_reader.open("tankWarConf.ini", ios::in);
	for (string reader; reader != "[file_end]"; file_reader >> reader)
	{
		if (reader != "")
		{
			setting_file.append(reader);
			setting_file.append("\r\n");
		}
	}
	file_reader.close();

	Cfind_in_ini ini_finder(setting_file);
	enemy_tank_paths = ini_finder.find_paths(1);
	brick_map = ini_finder.find_map(1);

	for (int i = 0; i < 13; i++)			//创建敌方坦克
	{
		enemy_tanks.push_back(Ctank(enemy_tanks_original_positions[i].x, enemy_tanks_original_positions[i].y, DIR_BACK, 30, 20, SIDE_ENEMY, 31, enemy_tank_paths[i]));
	}
	enemy_tanks_filled = true;

	vector <position> brick_side;
	for (int y = 0; y < 19; y++)
	{
		for (int x = 0; x < 19; x++)
		{
			if (brick_map[y][x] == true)
			{
				bricks.push_back(Cbrick(50 + x * (100 + 5), (2000 - y * (100 + 5)) - 50));

				brick_side.push_back({ bricks.back().brick_position.x + bricks.back().width, bricks.back().brick_position.y + bricks.back().height });
				brick_side.push_back({ bricks.back().brick_position.x - bricks.back().width, bricks.back().brick_position.y + bricks.back().height });
				brick_side.push_back({ bricks.back().brick_position.x + bricks.back().width, bricks.back().brick_position.y - bricks.back().height });
				brick_side.push_back({ bricks.back().brick_position.x - bricks.back().width, bricks.back().brick_position.y - bricks.back().height });
				brick_side.push_back({ bricks.back().brick_position.x, bricks.back().brick_position.y - bricks.back().height });
				brick_side.push_back({ bricks.back().brick_position.x, bricks.back().brick_position.y + bricks.back().height });
				brick_side.push_back({ bricks.back().brick_position.x + bricks.back().width, bricks.back().brick_position.y});
				brick_side.push_back({ bricks.back().brick_position.x - bricks.back().width, bricks.back().brick_position.y});
				for (int i = 0; i < brick_side.size(); i++)
				{
					if (tank_self.position_test(brick_side[i]))
					{
						bricks.pop_back();
					}

					for (int i = 0; i < enemy_tanks.size(); i++)
					{
						if (enemy_tanks[i].position_test(brick_side[i]))
						{
							bricks.pop_back();
						}
					}
				}
			}
		}
	}

	return 0;
}