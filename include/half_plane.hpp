// Определение класса полуплоскости и функции пересечения полуплоскостей

#ifndef INCLUDE_HALF_PLANE_HPP_
#define INCLUDE_HALF_PLANE_HPP_

#include "../include/edge.hpp"	// вообще говоря содержит только прототипы, но в этом файле подключается файл с реализацией edge_impl.hpp
#include "../include/point.hpp"
#include "../include/polygon.hpp"
#include <iostream>
#include <vector>

namespace geometry {
	double precision = 1e-7;

	class HalfPlane {
	private:
		double a, b, c;

		Edge<double> boundary;// эта прямая ax+by+c=0 - граница полуплоскости. Полуплоскость имеет уравнение ax+by+c>=0

	public:
		// по-умолчанию полуплоскость задаем как x >= 0, т.е. boundary проходит через (0, 0), (1, 0), а вектор нормали (1, 0)
		HalfPlane():	
			HalfPlane(Edge<double> (Point<double> (0, 0), Point<double> (0, 1)), Point<double> (1, 0))
		{}

		Edge<double> Boundary() const { return boundary; }

		// задаем полуплоскость по прямой и выбору нормали
		HalfPlane(Edge<double> _boundary, Point<double> normal) :
			boundary(_boundary)
		{
			double x1, y1, x2, y2, det;

			x1 = boundary.Origin().X();
			y1 = boundary.Origin().Y();
			x2 = boundary.Destination().X();
			y2 = boundary.Destination().Y();

			// находим a, b, c для данной boundary
			// условие det = 0 означает совпадение Origin и Destination у boundary
			// а это недоразумение мы поручаем контролировать самому boundary, т.е. здесь точно det != 0
			det = x1 * y2 - x2 * y1; 
			
			if (std::fabs(x2 - x1) < precision) {	// тогда x1=x2 и граница это вертикальная прямая x = x1
				a = 1;
				b = 0;
				c = -x1;
			}
			else if (std::fabs(y2 - y1) < precision) {	// тогда y1=y2  и граница это горизонтальная прямая y=y1
				a = 0;
				b = 1;
				c = -y1;
			}
			else { // значит x1 != x2, y1 != y2 и можем записать уравнение прямой x-x1 / x2-x1 = y-y1 / y2-y1
				a = y2 - y1;
				b = -(x2 - x1);
				c = -det;
			}
			
			// согласуем уравнение полуплоскости ax+by+c>=0, выбрав один из двух вариантов для полуплоскости, зная нормаль
			Point<double> point;
			point = boundary.Origin() + normal;
			
			if (!ContainPoint(point)) { // значит мы ошиблись уравнением ax+by+c>=0, и надо выбрать -ax-by-c>=0
				a *= (-1);
				b *= (-1);
				c *= (-1);
			}
		}

		bool ContainPoint(Point<double> point) { // возвращает: содержит ли полуплоскость точку?
			if (a * point.X() + b * point.Y() + c >= 0)
				return true;
			else
				return false;
		}

		/*void PrintEquation() {	// убрать для github
			std::cout << "equation for half plane: ";
			std::cout << a << "x + " << b << "y + " << c << " >= 0" << std::endl;
		}*/
	};



	/*void DrawPolygonOnScreen(std::vector<Point<double>> verticies) {	// убрать для github
		int size = verticies.size();
			int* tmpArray = new int[2 * (size + 1)];

			for (int i = 0; i < size + 1; i++) {
				if (i != size) {
					tmpArray[2 * i] = verticies[i].X();
					tmpArray[2 * i + 1] = verticies[i].Y();
				}
				else if (i == size) {
					tmpArray[2 * i] = verticies[0].X();
					tmpArray[2 * i + 1] = verticies[0].Y();
				}
			}

			initwindow(500, 500);
			fillpoly(size + 1, tmpArray);
			getch();
			closegraph();

			delete[] tmpArray;
			return;
	}*/

	Polygon<double, std::vector<Point<double>>> HalfPlaneIntersection(std::vector <HalfPlane> halfPlanes) {
		double boundarySize = 500;	// вся наша плоскость это квадрат со стороной 500
		int size;	// размер вектора ans
		Point<double> prev, next;	// prev, next - это текущее ребро 
		Point<double> mid;			// mid - пересечение ребра с границей полуплоскости (если есть пересечение)

		// это текущее пересечение полуплоскостей - многоугольник, хранящийся в виде вектора точек
		// мы будем пересекать квадрат с полуплоскостями, отсекая ненужное от квадрата
		std::vector <Point<double>> currentAnswerPolygon = { 
			Point<double>(0, 0), 
			Point<double>(boundarySize, 0), 
			Point<double>(boundarySize, boundarySize), 
			Point<double>(0, boundarySize) 
		};

		std::vector <Point<double>> newAnswerPolygon;	// результат пересечения многоугольника ans с новой полуплоскостью
		HalfPlane currentHalfPlane;

		for (HalfPlane currentHalfPlane : halfPlanes) {	// пересекаем многоугольник ans и currentHalfPlane

			// тут происходит иногда ошибка - дублируется вершина, из-за чего возникает ребро нулевой длины и деление на ноль
			// дублирование вершины происходит из-за того, что граница плоскости проходит ровненько через вершину многоугольника

			newAnswerPolygon.resize(0);
			int size = currentAnswerPolygon.size();

			for (int i = 0; i < size; i++) {	// пересекаем границу полуплоскости с ребром (prev, next)
				if (i != size - 1) {
					prev = currentAnswerPolygon[i];
					next = currentAnswerPolygon[i + 1];
				}
				else {
					prev = currentAnswerPolygon[size - 1];
					next = currentAnswerPolygon[0];
				}

				// пересекаем две прямые: Edge(prev, next) и границу currentHalfPlane
				double t; // в эту переменную запишется координата точки на ребре, соответствующая пересечению
				Intersection intersectionType;	// сюда запишется тип пересечения - параллельны?
				// parallel - параллельны. collinear - одна и та же прямая - для простоты считаем это одним случаем
				intersectionType = Edge<double>(prev, next).Intersect(currentHalfPlane.Boundary(), &t, precision);
				mid = prev + t * (next - prev);

				if (intersectionType != Intersection::Parallel && intersectionType != Intersection::Collinear) {
					if (currentHalfPlane.ContainPoint(prev) && currentHalfPlane.ContainPoint(next))
						newAnswerPolygon.push_back(next);	// полуплоскость содержит целиком ребро (prev, next). Точка mid не нужна
					else if (!currentHalfPlane.ContainPoint(prev) && !currentHalfPlane.ContainPoint(next)) {
					}	// полуплоскость целиком не содержит ребро (prev, next). Не делаем ничего
						// в остальных случаях полуплоскость точно пересекается с ребром точно по точке
					else if (currentHalfPlane.ContainPoint(prev) && !currentHalfPlane.ContainPoint(next))
						newAnswerPolygon.push_back(mid);
					else if (!currentHalfPlane.ContainPoint(prev) && currentHalfPlane.ContainPoint(next)) {
						newAnswerPolygon.push_back(mid);
						newAnswerPolygon.push_back(next);
					}
				}
				else if (intersectionType == Intersection::Parallel || intersectionType == Intersection::Collinear) {
					if (currentHalfPlane.ContainPoint(next))
						newAnswerPolygon.push_back(next);
				}


			}


			currentAnswerPolygon = newAnswerPolygon;

			// DrawPolygonOnScreen(currentAnswerPolygon);	// убрать для github

			newAnswerPolygon.resize(0);
		}
		return Polygon<double, std::vector<Point<double>> >(currentAnswerPolygon);

	}


} // namespace geometry

#endif // INCLUDE_HALF_PLANE_HPP_
