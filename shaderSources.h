/* 此头文件定义了着色器源码 */
#pragma once

//定义着色器的源码
const char* vertexShaderSource_graphic = "#version 330 core\n"			//顶点着色器源码
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource_red = "#version 330 core\n"	//片段着色器源码（红）
"out vec4 FragColor;\n"
"void main()\n"
"{\n"				//red, gren, blue
"	FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\0";

const char* fragmentShaderSource_blue = "#version 330 core\n"	//片段着色器源码（蓝）
"out vec4 FragColor;\n"
"void main()\n"
"{\n"				//red, gren, blue
"	FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);\n"
"}\0";

const char* fragmentShaderSource_wite = "#version 330 core\n"	//片段着色器源码（白）
"out vec4 FragColor;\n"
"void main()\n"
"{\n"				//red, gren, blue
"	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
"}\0";

const char* fragmentShaderSource_grea = "#version 330 core\n"	//片段着色器源码（灰）
"out vec4 FragColor;\n"
"void main()\n"
"{\n"				//red, gren, blue
"	FragColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);\n"
"}\0";

const char* fragmentShaderSource_black = "#version 330 core\n"	//片段着色器源码（黑）
"out vec4 FragColor;\n"
"void main()\n"
"{\n"				//red, gren, blue
"	FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
"}\0";

const char* fragmentShaderSource_yellow = "#version 330 core\n"	//片段着色器源码（黄）
"out vec4 FragColor;\n"
"void main()\n"
"{\n"				//red, gren, blue
"	FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
"}\0";

const char* vertexShaderSource_text = "#version 330 core\n"
"layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\n"
"out vec2 TexCoords;\n"
"\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
"TexCoords = vertex.zw;\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"\n"
"uniform sampler2D text;\n"
"uniform vec3 textColor;\n"
"\n"
"void main()\n"
"{\n"
"vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
"color = vec4(textColor, 1.0) * sampled;\n"
"}\0";