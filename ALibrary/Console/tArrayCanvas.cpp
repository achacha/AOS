#include <templateAArrayCanvas.hpp>
#include <iostream>

void testRect()
{
	AArrayCanvas<u4> canvas(10,10,' ');
	canvas.drawRect(0,0,2,2,u4('A'));
	canvas.drawRect(9,9,5,6,u4('A'));

	for (u2 y=0;	y<canvas.getSizeY(); ++y) {
		for (u2 x=0; x<canvas.getSizeX(); ++x) {
			std::cout << (char)canvas.at(x,y);
		}
		std::cout << std::endl;
	}
}

void testLineThin()
{
	AArrayCanvas<u1> canvas(10,10,' ');
	canvas.drawLine(9,9,0,0,'1',false);
	canvas.drawLine(8,2,8,5,'2',false);
	canvas.drawLine(2,7,6,4,'3',false);

	for (u2 y=0;	y<canvas.getSizeY(); ++y) {
		for (u2 x=0; x<canvas.getSizeX(); ++x) {
			std::cout << (char)canvas.at(x,y);
		}
		std::cout << std::endl;
	}
}

void testLineThick()
{
	AArrayCanvas<u1> canvas(10,10,' ');
	canvas.drawLine(9,9,0,0,'1');
	canvas.drawLine(8,2,8,5,'2');
	canvas.drawLine(2,7,6,4,'3');

	for (u2 y=0;	y<canvas.getSizeY(); ++y) {
		for (u2 x=0; x<canvas.getSizeX(); ++x) {
			std::cout << (char)canvas.at(x,y);
		}
		std::cout << std::endl;
	}
}

int main()
{
//	testRect();
	testLineThin();
	testLineThick();

	return 0;
}
