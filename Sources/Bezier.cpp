#include "Bezier.h"
#include <cmath>

Bezier::Bezier(
	const std::vector<sf::Vector2f>& points,
	const float& timing,
	const size_t nbSegments,
	const sf::Color& color,
	const bool isClosed)
{
    std::cout << "Construct new Bezier curve" << std::endl;

    if (points.size() < 2)
        std::cerr << "Bezier curve needs at lest two points to be generated" << std::endl;

    this->_isClosed = isClosed;
	this->_color = color;
    this->_nbSegments = nbSegments;
	this->_timing = sf::seconds(timing);
    this->_points = {};
    this->_controlPoints = {};
    this->_anchorPoints = {};
	this->_length = 0;

    // Store user's points
    for (auto point : points)
		// Add point and translate each points to [0 ; 0]
        this->addPoint(point - points[0]);

    // Create control points based on user's points
    // Then create anchor points based on control points
    // Finally create curve vertices based on every elements
    if (this->_points.size() >= 2)
        this->update();

	this->_nbMaxVertices = this->_bezierCurve.size();

	// Calculate the curve's length (in pixel)
	for (size_t i = 1; i < this->_bezierCurve.size(); i++) {
		sf::Vector2f vec = this->_bezierCurve[i].position - this->_bezierCurve[i - 1].position;

		this->_length += sqrt(pow(vec.x, 2) + pow(vec.y, 2));
	}

	std::cout << "Curve length : " << this->_length << std::endl;

    std::cout << "Done !" << std::endl << std::endl;
}

// GETTERS
const bool      Bezier::isClosed() const
{
    return (this->_isClosed);
}

const size_t    Bezier::getSegments() const
{
	return (this->_nbSegments);
}

const size_t    Bezier::getMaxVertices() const
{
	return (this->_nbMaxVertices);
}

const std::vector<sf::Vector2f>&  Bezier::getPoints() const
{
    return (this->_points);
}

const std::vector<sf::Vector2f>&  Bezier::getControlPoints() const
{
    return (this->_controlPoints);
}

const std::vector<sf::Vector2f>&  Bezier::getAnchorPoints() const
{
    return (this->_anchorPoints);
}

std::vector<sf::Vertex>&	Bezier::getBezierCurve()
{
    return (this->_bezierCurve);
}

const sf::Time&	Bezier::getTiming() const
{
	return (this->_timing);
}

const float&	Bezier::getPixelLength() const
{
	return (this->_length);
}


// SETTERS
void    Bezier::setClose(const bool isClosed)
{
    this->_isClosed = isClosed;
}

void    Bezier::setSegments(const size_t nbSegments)
{
    this->_nbSegments = nbSegments;
}

void    Bezier::setPointById(const int id, const sf::Vector2f& pos)
{
    this->_points[id] = pos;
}

void	Bezier::setTiming(const float timing)
{
	this->_timing = sf::seconds(timing);
}


// METHODS
void Bezier::addPoint(const sf::Vector2f& point)
{
    std::cout << "Add point: [" << point.x << " ; " << point.y << "]" << std::endl;
    this->_points.push_back(point);
}

void Bezier::addTiming(const float& offset)
{
	this->_timing += sf::seconds(offset);
}

void Bezier::removeVertex(const std::vector<sf::Vertex>::const_iterator it_point)
{
	this->_bezierCurve.erase(it_point);
}

void Bezier::addControlPoint(const sf::Vector2f& point)
{
    std::cout << "Add control point: [" << point.x << " ; " << point.y << "]" << std::endl;
    this->_controlPoints.push_back(point);
}

void Bezier::addAnchorPoint(const sf::Vector2f& point)
{
    std::cout << "Add anchor point: [" << point.x << " ; " << point.y << "]" << std::endl;
    this->_anchorPoints.push_back(point);
}

void Bezier::updateControlPoints()
{
    // Create control points dynamically
    for (int i = 0; i < this->_points.size() - 1; i++) {
        sf::Vector2f cpoint = this->_points[i + 1] - this->_points[i];

        this->addControlPoint(sf::Vector2f(
            this->_points[i].x + cpoint.x / 3.f,
            this->_points[i].y + cpoint.y / 3.f));
        this->addControlPoint(sf::Vector2f(
            this->_points[i].x + 2 * cpoint.x / 3.f,
            this->_points[i].y + 2 * cpoint.y / 3.f));
    }

    if (this->_isClosed) {
        sf::Vector2f lastPoint = this->_points[this->_points.size() - 1];
        sf::Vector2f delta = this->_points[0] - lastPoint;

        this->addControlPoint(sf::Vector2f(lastPoint.x + delta.x / 3.f, lastPoint.y + delta.y / 3.f));
        this->addControlPoint(sf::Vector2f(lastPoint.x + 2 * delta.x / 3.f, lastPoint.y + 2 * delta.y / 3.f));
    }
}

void Bezier::updateAnchorPoints()
{
    if (this->_controlPoints.size() > 2) {
        if (this->_isClosed) {
            sf::Vector2f lastControl = this->_controlPoints[this->_controlPoints.size() - 1];
            this->addAnchorPoint(sf::Vector2f(
                (this->_controlPoints[0].x + lastControl.x) / 2.f,
                (this->_controlPoints[0].y + lastControl.y) / 2.f));
        }
        else
            this->addAnchorPoint(this->_points[0]);

        // Use new control points to create anchors
        for (int i = 0, j = 1; i < this->_points.size() - 1 - !this->_isClosed; i++, j += 2) {
            this->addAnchorPoint(sf::Vector2f(
                (this->_controlPoints[j].x + this->_controlPoints[j + 1].x) / 2,
                (this->_controlPoints[j].y + this->_controlPoints[j + 1].y) / 2
            ));
        }

        if (this->_isClosed)
            this->addAnchorPoint(this->_anchorPoints[0]);
        else
            this->addAnchorPoint(this->_points[this->_points.size() - 1]);
    }
}

void Bezier::updateBezierCurve()
{
    std::cout << "Get bezier vertices..." << std::endl;
    int j = 0;

    for (int i = 0, j = 0; i < (int)this->_anchorPoints.size() - 1; i++, j += 2) {
        std::cout << "Create vertice no " << i << std::endl;
        this->cubicBezierVertex(
            this->_anchorPoints[i], this->_anchorPoints[i + 1],
            this->_controlPoints[j], this->_controlPoints[j + 1]);
    }
}

void Bezier::update()
{
    this->_controlPoints.clear();
    this->_anchorPoints.clear();
    this->_bezierCurve.clear();

    this->updateControlPoints();
    this->updateAnchorPoints();
    this->updateBezierCurve();
}

void Bezier::cubicBezierVertex(
    const sf::Vector2f& startAnchor,
    const sf::Vector2f& endAnchor,
    const sf::Vector2f& startControl,
    const sf::Vector2f& endControl)
{
    std::vector<sf::Vector2f> ret;
    if (this->_nbSegments <= 0)
        return;

    float t = 0;

    // Calculate point with numSegements precision
    // Bezier formula
    for (size_t i = 0; i <= this->_nbSegments; i++, t += 1.f / this->_nbSegments) {
        ret.push_back(std::pow(1.f - t, 3.f) * startAnchor +
            3.f * std::pow(1.f - t, 2.f) * t * startControl +
            3.f * (1.f - t) * std::pow(t, 2.f) * endControl +
            std::pow(t, 3.f) * endAnchor);
    }

	for (auto curve : ret)
		this->_bezierCurve.push_back(sf::Vertex(curve, this->_color));
}