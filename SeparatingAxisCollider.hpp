#pragma once
#include "Basis.hpp"
#include "Collider.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <set>



/*
Separating Axis Theorem для двух выпуклых объектов можно сформулировать так:
два выпуклых объекта пересекаются тогда и только тогда, когда существует плоскость
(для двумерного случая - прямая), такая, что одна геометрия лежит по одну ей сторону,
а другая - по другую.
*/
class SeparatingAxisCollider : Collider {

    std::vector<std::shared_ptr<Object>> objectsToCollide;

    // Получаем ребра многоугольника
    std::vector<Line> getPolygonSides(const Polygon& polygon) const {
        auto numberOfPoints = polygon.getPointCount();

        std::vector<Line> lines;
        lines.reserve(numberOfPoints);

        auto getLineFormPoints = [&](std::size_t startIndex, std::size_t endIndex) {
            auto start = polygon.getRealPoint(startIndex);
            auto end = polygon.getRealPoint(endIndex);
            lines.emplace_back(start, end);
        };

        for (std::size_t i = 0; i < numberOfPoints - 1; ++i) {
            getLineFormPoints(i, i + 1);
        }

        getLineFormPoints(numberOfPoints - 1, 0);

        return lines;
    }

    std::vector<Line::LineCoefficients> getPolygonSidesNormalCoefficients(const Polygon& polygon) const {
        auto numberOfPoints = polygon.getPointCount();

        if (numberOfPoints < 3) {
            throw std::logic_error(
                "Unexpected number of points in polyugon: "
                + std::to_string(numberOfPoints)
            );
        }

        auto lines = getPolygonSides(polygon);

        std::vector<Line::LineCoefficients> axises;
        axises.reserve(numberOfPoints);

        for (const Line line : lines) {
            axises.push_back(line.getNormalCoefficients());
        }

        return axises;
    }

    bool isLineIntersected(sf::Vector2f a, sf::Vector2f b) const {

        if (b.y >= a.x && b.x <= a.y) {
            return true;
        }

        return false;
    }

    sf::Vector2f getPolygonProjection(const Polygon& polygon, const Line::LineCoefficients axis) const {

        auto numberOfPoints = polygon.getPointCount();

        std::vector<float> projections;
        projections.reserve(numberOfPoints);

        for (std::size_t i = 0; i < numberOfPoints; ++i) {
            auto point = polygon.getRealPoint(i);
            auto pointProjection = getPointProjectionMod(point, axis);
            projections.push_back(pointProjection);
        }

        auto x = *std::ranges::min_element(projections);
        auto y = *std::ranges::max_element(projections);

        return {x, y};
    }

    Point getPointProjectionReal(const Point& point, const Line::LineCoefficients axis) const {

        auto xProjection = point.x - (axis.a * ((axis.a * point.x + axis.b * point.y + axis.c) / (axis.a * axis.a + axis.b * axis.b)));
        auto yProjection = point.y - (axis.b * ((axis.a * point.x + axis.b * point.y + axis.c) / (axis.a * axis.a + axis.b * axis.b)));

        return {xProjection, yProjection};
    }

    float getPointProjectionMod(const Point& point, const Line::LineCoefficients axis) const {
        sf::Vector2f vector{-axis.b, axis.a};
        auto vectorAbs = std::sqrt(vector.x * vector.x + vector.y * vector.y);
        return (point.x * vector.x + point.y * vector.y) / vectorAbs;
    }

    bool isIntersect(const Polygon& a, const Polygon& b) {

        auto aAxis = getPolygonSidesNormalCoefficients(a);
        auto bAxis = getPolygonSidesNormalCoefficients(b);

        std::vector<Line::LineCoefficients> axisToCheck;
        axisToCheck.reserve(aAxis.size() + bAxis.size());

        std::copy(aAxis.cbegin(), aAxis.cend(), std::back_inserter(axisToCheck));
        std::copy(bAxis.cbegin(), bAxis.cend(), std::back_inserter(axisToCheck));

        for (auto axis : axisToCheck) {
            auto aProjection = getPolygonProjection(a, axis);
            auto bProjection = getPolygonProjection(b, axis);

            if (!isLineIntersected(aProjection, bProjection)) {
                return false;
            }
        }

        return true;
    }



public:

    void loadObjects(const std::vector<std::shared_ptr<Object>>& objects) override {
        objectsToCollide.reserve(objects.size());
        for (auto obj : objects) {
            objectsToCollide.push_back(obj);
        }
    }

    void loadObject(const std::shared_ptr<Object>& object) override {
        objectsToCollide.push_back(object);
    }

    void removeObject(const std::shared_ptr<Object>& object) override {
        //objectsToCollide.erase(object);
    }

    std::map<Object::TId, std::vector<Object::TId>> getIntersections() override {
        auto numberOfObjects = objectsToCollide.size();

        std::map<Object::TId, std::vector<Object::TId>> result;

        for (auto objectIndex = 0; objectIndex < numberOfObjects - 1; objectIndex++) {
            for (auto collidedObjectIndex = objectIndex + 1; collidedObjectIndex < numberOfObjects; collidedObjectIndex++) {

                const auto& object = *objectsToCollide[objectIndex];
                const auto& objectToCollide = *objectsToCollide[collidedObjectIndex];

                if (isIntersect(object, objectToCollide)) {
                    result[object.id].push_back(objectToCollide.id);
                }
            }
        }

        return result;
    }
};