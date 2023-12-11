#include <SFML/Graphics.hpp>
#include <ctime>
#include <iostream>

using namespace sf;
using namespace std;

const int W = 10;
const int H = 20;
int field[H][W] = {0};
struct Point
{
	int x, y;
};



class Shaper {

	int figures[7][4] =
	{
		1,3,5,7, // I
		2,4,5,7, // Z
		3,5,4,6, // S
		3,5,4,7, // T
		2,3,5,7, // L
		3,5,7,6, // J
		2,3,4,5, // O
	};
	int index;
	int partTexture[4] = { 0, 0, 18, 18 };
public:
	int color_num = index + 1;
	Point p[4], previous[4]{0};
	Shaper() = default;
	~Shaper() {
	};
	void generateShape() {
		srand(time(0));
		index = rand() % 7;
		for (int i = 0; i < 4; i++)
		{
			p[i].x = figures[index][i] % 2;
			p[i].y = figures[index][i] / 2;
		}

	}
	int* getPartTexture() {
		partTexture[0] = index * 18;
		return partTexture;
	}
	void save_prev_coord() {
		for (size_t i = 0; i < 4; i++) {
			previous[i].x = p[i].x;
			previous[i].y = p[i].y;
		}
	}
	void undo_coord() {
		for (size_t i = 0; i < 4; i++) {
			p[i] = previous[i];
		}
	}
	void moveRight() {
		save_prev_coord();
		for (size_t i = 0; i < 4; i++) {
			p[i].x += 1;
		}
		if (!checkSideBorders()) undo_coord();
		
	}
	void moveLeft() {
		save_prev_coord();
		for (size_t i = 0; i < 4; i++) {
			p[i].x -= 1;
		}
		if (!checkSideBorders()) undo_coord();
	}
	void rotate() {
		Point newPositions[4];
		Point zero = p[1];
		for (size_t i = 0; i < 4; i++)
		{
			int x = p[i].y - zero.y;
			int y = p[i].x - zero.x;
			newPositions[i].x = zero.x - x;
			newPositions[i].y = zero.y + y;
		}
		bool canRotate = true;
		for (size_t i = 0; i < 4; i++) {
			if (newPositions[i].x < 0 || newPositions[i].x >= W ||
				newPositions[i].y < 0 || newPositions[i].y >= H ||
				field[newPositions[i].y][newPositions[i].x]) {
				canRotate = false;
				break;
			}
		}
		if (canRotate) {
			for (size_t i = 0; i < 4; i++) {
				p[i] = newPositions[i];
			}
		}
	}
	void moveDown() {
		save_prev_coord();
		if(checkBorders())
		for (size_t i = 0; i < 4; i++) {
			p[i].y += 1;
		}
	}
	int getIndex() const{
		return index;
	}
	bool checkBorders() {
		for (size_t i = 0; i < 4; i++)
		{
			if (p[i].y >= H) return 0;
			else if (field[p[i].y][p[i].x]) return 0;
		}
		return 1;
	}
	bool checkSideBorders() {
		for (size_t i = 0; i < 4; i++)
		{
			if (p[i].x < 0 || p[i].x >= W) return 0;
			else if (field[p[i].y][p[i].x]) return 0;
		}
		return 1;
	}
};

class ShapeFactory {
public:
	static Shaper createShape() {
		Shaper newShape;
		newShape.generateShape();
		return newShape;
	}
};


class Matrix 
{
private:


	Texture texture, texture_background, texture_frame;
	Sprite sprite, sprite_background, sprite_frame;
	int score;
public:
	bool beginGame = true;
	RenderWindow window;

	Matrix(int width, int height, string name) : score { 0 }, window(VideoMode(width, height), name) {
	};
	~Matrix() {};

	void loadTexture(string filename) {
		texture.loadFromFile(filename);
		
	}
	void loadBackground(string filename) {
		texture_background.loadFromFile(filename);
		sprite_background = Sprite(texture_background);
	}
	void load_frame(string filename) {
		texture_frame.loadFromFile(filename);
		sprite_frame = Sprite(texture_frame);
	}

	void selectSprite(int left, int top, int width, int height) {
		sprite = Sprite(texture);
		sprite.setTextureRect(IntRect(left, top, width, height));
	}
	void selectSprite(int arr[4]) {
		sprite = Sprite(texture);
		sprite.setTextureRect(IntRect(arr[0], arr[1], arr[2], arr[3]));
	}
	void setSpritePos(int x, int y) {
		sprite.setPosition(x,y);
	}
	void moveSprite(int x, int y) {
		sprite.move(x, y);
	}
	void drawSprite() {
		window.draw(sprite);
	}
	void drawBackground() {
		window.draw(sprite_background);
	}
	void drawFrame() {
		window.draw(sprite_frame);
	}
	void setBackground(Color c) {
		window.clear(c);
	}
	void drawShape(const Shaper& shape ){
		for (int i = 0; i < 4; i++) {
			setSpritePos(shape.p[i].x * 18, shape.p[i].y * 18);
			moveSprite(28, 31);
			drawSprite();
		}
	}
	void drawSavedShapes() {
		for (int i = 0; i < H; i++) {
			for (int j = 0; j < W; j++) {
				if (field[i][j] == 0) continue;
				selectSprite((field[i][j]-1) * 18, 0, 18, 18);
				setSpritePos(j * 18, i * 18);
				moveSprite(28, 31);
				drawSprite();
			}
		}
	}
	void remove_line() {
		int k = H - 1;
		for (int i = H - 1; i > 0; i--)
		{
			int count = 0;
			for (int j = 0; j < W; j++)
			{
				if (field[i][j]) count++;
				field[k][j] = field[i][j];
			}
			if (count < W) k--;
		}
	}
};

bool checkTop() {
	for (size_t i = 0; i < W; i++)
	{
		if (field[1][i] != 0) return true;
	}
	return false;
}

float timer = 0, delay = 0.3;
Clock tclock;

int main()
{
	Matrix app(320, 480, "Tetris");
	app.loadTexture("C://Users//user//Desktop//ØÀÃ//C++//Tetris//images//tiles.png");
	app.loadBackground("C://Users//user//Desktop//ØÀÃ//C++//Tetris//images//background.png");
	app.load_frame("C://Users//user//Desktop//ØÀÃ//C++//Tetris//images//frame.png");
	Shaper* shape = nullptr;
	while (app.window.isOpen())
	{

		float time = tclock.getElapsedTime().asSeconds();
		tclock.restart();
		timer += time;
		if (app.beginGame)
		{
			app.beginGame = false;
			Shaper shaper = ShapeFactory::createShape();
			shape = &shaper;
			app.selectSprite(shape->getPartTexture());
		}

		Event event;
		while (app.window.pollEvent(event))
		{
			if (event.type == Event::Closed) {
				app.window.close();
			}
			if (event.type == Event::KeyPressed)
				switch (event.key.code)
				{
				case Keyboard::Up:
					shape->rotate();
					break;
				case Keyboard::Left:
					shape->moveLeft();
					break;
				case Keyboard::Right:
					shape->moveRight();
					break;
				}
		}
		if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.1;

		if (timer > delay)
		{
			shape->moveDown();
			if (!checkTop())
			if (!shape->checkBorders())
			{
					const int color = shape->getIndex() + 1;
					for (size_t i = 0; i < 4; i++)
					{
						if (field[shape->previous[i].y][shape->previous[i].x] == 0) {
							field[shape->previous[i].y][shape->previous[i].x] = color;
						}
					}
					//for (size_t i = 0; i < H; i++)
					//{
					//	for (size_t j = 0; j < W; j++) {
					//		cout << field[i][j] << "\t";
					//	}
					//	cout << endl;
					//}
					//cout << endl;
					Shaper new_shaper = ShapeFactory::createShape();
					shape = &new_shaper;
					app.selectSprite(shape->getPartTexture());
					delay = 0.3;
			}
			timer = 0;
			
		}
		app.remove_line();

		app.setBackground(Color::Black);
		app.drawBackground();
		app.drawSavedShapes();
		app.drawShape(*shape);
		app.drawFrame();
		app.window.display();
	}
}