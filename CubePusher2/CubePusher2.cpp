// CubePusher2.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

void readFile(char** buffer, int* size, const char* filename) {
	ifstream in(filename);
	if (!in) {
		*buffer = 0;
		*size = 0;
	}
	else {
		in.seekg(0, ifstream::end);
		*size = static_cast<int>(in.tellg());
		in.seekg(0, ifstream::beg);
		*buffer = new char[*size];
		in.read(*buffer, *size);
	}
}

template<typename T>
class Array2d {
public:
	Array2d() :data(0) {}
	~Array2d() {
		delete[] data;
		data = 0;
	}
	void setSize(int size0, int size1) {
		mSize0 = size0;
		mSize1 = size1;
		data = new T[size0*size1];
	}
	T& operator()(int x, int y) {
		return data[y*mSize0 + x];
	}
	const T& operator()(int x, int y)const{
		return data[y*mSize0 + x];
	}	
private:
	T* data;
	int mSize0;
	int mSize1;
};

class State {
public:
	State(const char* stageData, int size);
	void update(char input);
	void draw() const;
	bool checkClear() const;
private:
	enum Object {
		OBJ_WALL,OBJ_SPACE,OBJ_MAN,OBJ_BLOCK,OBJ_UNKNOWN,
	};
	void setSize(const char* stageData, int size);
	int mWidth;
	int mHeight;
	Array2d<Object> mObjects;
	Array2d<bool> mGoalFlags;
};

int main(int argc, char** argv)
{
	const char* filename = "stage.txt";
	if (argc >= 2) {
		filename = argv[1];
	}
	char* stageData;
	int fileSize = 0;
	readFile(&stageData, &fileSize, filename);
	if (!stageData) {
		cout << "stage file could not be read." << endl;
		return 1;
	}
	State* state = new State(stageData, fileSize);

	//主循环
	while (true) {
		//
		state->draw();
		//通关检测
		if (state->checkClear()) {
			break; //通关检测
		}
		//提示如何操作
		cout << "a:left s:right w:up z:down. command?" << endl; //操作说明
		char input;
		cin >> input;
		//刷新
		state->update(input);
	}
	//打印通关祝贺的信息
	cout << "Congratulation's! you won." << endl;
	//析构
	delete[] stageData;
	stageData = 0;

	//无限循环（ctrl-C中断）
	while (true) {
		char x;
		cin >> x;
		break;
	}
	return 0;
}

State::State(const char * stageData, int size)
{	
	setSize(stageData, size);
	mObjects.setSize(mWidth, mHeight);
	mGoalFlags.setSize(mWidth, mHeight);
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mObjects(x, y) = OBJ_WALL;
			mGoalFlags(x, y) = false;
		}
	}
	int x = 0;
	int y = 0;
	for (int i = 0; i < size; ++i) {
		Object t;
		bool goalFlag = false;
		switch (stageData[i]) {
		case '#': t = OBJ_WALL; break;
		case ' ': t = OBJ_SPACE; break;
		case '.': t = OBJ_SPACE; goalFlag = true; break;
		case 'o': t = OBJ_BLOCK; break;
		case 'O': t = OBJ_BLOCK; goalFlag = true; break;
		case 'p': t = OBJ_MAN; break;
		case 'P': t = OBJ_MAN; goalFlag = true; break;
		case '\n': t = OBJ_UNKNOWN; x = 0; ++y; break;
		default: t = OBJ_UNKNOWN; break;
		}
		if (t != OBJ_UNKNOWN) {
			mObjects(x, y) = t;
			mGoalFlags(x, y) = goalFlag;
			++x;
		}
	}
}

void State::update(char input)
{
	int dx = 0;
	int dy = 0;
	switch (input) {
	case 'a': dx = -1; break;
	case 's': dx = 1; break;
	case 'w': dy = -1; break;
	case 'z': dy = 1; break;
	}	
	//使用较短的变量名
	int w = mWidth;
	int h = mHeight;
	Array2d<Object>& o = mObjects;
	//查找小人的坐标	
	int x = 0;
	int y = 0; 
	bool found = false;
	for (; y < mHeight; ++y) {
		for ( x =0 ; x < mWidth; ++x) {
			if (o(x, y) == OBJ_MAN) {
				found = true;
				break;
			}
		}
		if (found) {
			break;
		}
	}
	int tx = x + dx;
	int ty = y + dy;
	if (tx < 0 || ty < 0 || tx >= w || ty >= h) {
		return;
	}
	if (o(tx, ty) == OBJ_SPACE) {
		o(tx, ty) = OBJ_MAN;
		o(x, y) = OBJ_SPACE;
	}else if (o(tx, ty) == OBJ_BLOCK) {
		int tx2 = tx + dx;
		int ty2 = ty + dy;
		if (tx2 < 0 || ty2 < 0 || tx2 >= w || ty2 >= h) { //按键无效
			return;
		}
		if (o(tx2, ty2) == OBJ_SPACE) {
			o(tx2, ty2) = OBJ_BLOCK;
			o(tx, ty) = OBJ_MAN;
			o(x, y) = OBJ_SPACE;
		}
	}
}

void State::draw() const
{
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			Object o = mObjects(x, y);
			bool goalFlag = mGoalFlags(x, y);
			if (goalFlag) {
				switch (o) {
				case OBJ_BLOCK: cout << 'O'; break;
				case OBJ_MAN:   cout << 'P'; break;
				case OBJ_SPACE: cout << '.'; break;
				case OBJ_WALL: cout << '#'; break;
				}
			}
			else {
				switch (o) {
				case OBJ_BLOCK:cout << 'o'; break;
				case OBJ_MAN: cout << 'p'; break;
				case OBJ_SPACE: cout << ' '; break;
				case OBJ_WALL:cout << '#'; break;
				}
			}
		}
		cout << endl;
	}
}

bool State::checkClear() const
{
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			if (mObjects(x, y) == OBJ_BLOCK) {
				if (!mGoalFlags(x, y)) {
					return false;
				}
			}
		}
	}
	return true;
}

void State::setSize(const char * stageData, int size)
{
	int x = 0;
	int y = 0;
	mHeight = 0;
	mWidth = 0;
	for (int i = 0; i < size; ++i) {
		switch (stageData[i]) {
		case '#': case ' ': case '.': case 'p': 
		case 'P': case 'O': case 'o':
			++x;
			break;
		case '\n':
			++y;
			mWidth = max(x, mWidth);
			mHeight = max(y, mHeight);
			x = 0;
			break;
		}
	}
}

