/* ��ͷ�ļ����������нṹ�� */
#pragma once
struct position_float			//��������Ϊfloat������ṹ
{
	float x;	//x����
	float y;	//y����
	float z;	//z����
};

struct position					//��������Ϊint������ṹ
{
	int x;	//x����
	int y;	//y����
	int z;	//z����
};

struct rec_data			//����������ݼܹ�
{
	float x_pos;	//x����
	float y_pos;	//y����
	float width;	//���
	float height;	//����
};

struct Character {
	GLuint     TextureID;  // ���������ID
	glm::ivec2 Size;       // ���δ�С
	glm::ivec2 Bearing;    // �ӻ�׼�ߵ�������/������ƫ��ֵ
	GLuint     Advance;    // ԭ�����һ������ԭ��ľ���
};