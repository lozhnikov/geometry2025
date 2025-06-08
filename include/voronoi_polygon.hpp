// Файл содержит реализацию алгоритма нахождения многоугольника Вороного

#ifndef VORONOI_POLYGON_HPP
#define VORONOI_POLYGON_HPP

#include <iostream>
#include <vector>
#include "../include/edge.hpp"	
#include "../include/point.hpp"
#include "../include/polygon.hpp"
#include "../include/half_plane.hpp"


// строит многоугольник Вороного для точки centerPoint относительно точек points
// надо построить серединные перпендикуляры centerPoint, points[i] и найти пересечение полуплоскостей
namespace geometry {
	geometry::Polygon<double, std::vector<Point<double>>> VoronoiPolygon(Point<double> centerPoint, std::vector <Point<double>> points) {
		int size = points.size();
		std::vector <HalfPlane> halfPlanes;
		HalfPlane currentHalfPlane;
		Point<double> currentNormal;
		Edge<double> edge;

		// создаем вектор из нужных полуплоскостей

		for (int i = 0; i < size; i++) {
			edge = Edge<double>(centerPoint, points[i]);
			edge = edge.Rotate();	// теперь это серединный перпендикуляр к отрезку centerPoint, points[i]


			Point<double> A = edge.Origin(), B = edge.Destination();

			// вектор нормали смотрит к точке centerPoint
			currentNormal = centerPoint - points[i];

			currentHalfPlane = HalfPlane(edge, currentNormal);

			// currentHalfPlane.PrintEquation();	// стереть для github

			halfPlanes.push_back(currentHalfPlane);
		}


		return HalfPlaneIntersection(halfPlanes);
	}
}

#endif // VORONOI_POLYGON_HPP