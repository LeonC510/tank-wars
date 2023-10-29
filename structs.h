/* 此头文件定义了所有结构体 */
#pragma once
struct position_float			//定义坐标为float的坐标结构
{
	float x;	//x坐标
	float y;	//y坐标
	float z;	//z坐标
};

struct position					//定义坐标为int的坐标结构
{
	int x;	//x坐标
	int y;	//y坐标
	int z;	//z坐标
};

struct rec_data			//定义矩形数据架构
{
	float x_pos;	//x坐标
	float y_pos;	//y坐标
	float width;	//宽度
	float height;	//长度
};

struct Character {
	GLuint     TextureID;  // 字形纹理的ID
	glm::ivec2 Size;       // 字形大小
	glm::ivec2 Bearing;    // 从基准线到字形左部/顶部的偏移值
	GLuint     Advance;    // 原点距下一个字形原点的距离
};