#include "Bezier.h"
#include <cmath>

Bezier::Bezier(
	const std::vector<sf::Vector2f>& points,
	const std::vector<float>& durations,
	const sf::Time& start,
	float& bpm,
	const sf::Color color,
	const size_t& nbSegments,
	const bool& isClosed) : _bpm(bpm)
{
	std::cout << "Construct new Bezier curve" << std::endl;

	if (points.size() < 1) {
		std::cerr << "Bezier curve needs at lest two points to be generated" << std::endl;
		return;
	}

    this->_isClosed = isClosed;
	this->_color = color;
    this->_nbSegments = nbSegments;
	this->_start = start;
	this->_duration = 0;
	this->_durations = durations;
    this->_points = {};
    this->_controlPoints = {};
    this->_anchorPoints = {};
	this->_pixel_length = 0;

	size_t userPointsIndex = 0;
	size_t index = 0;

	this->addPoint({ 0, 0 });

    // Store user's points
	for (auto point : points) {
		this->_duration += durations[userPointsIndex];

		for (float i = 0.25f; i <= durations[userPointsIndex]; i += 0.25f) {
			const sf::Vector2f subpoint = (point / durations[userPointsIndex]) * i;
			std::cout << "Subpoint " << i * 4.f << "= [" << subpoint.x << " ; " << subpoint.y << "]" << std::endl;
			// Set each point to a location based on BPM and curve duration
			// Add point and translate each points to [0 ; 0]
			this->addPoint(subpoint * 300.f * durations[userPointsIndex] + this->_points[index - (i * 4.f) + 1]);
			index++;
		}

		userPointsIndex++;
	}

    // Create control points based on user's points
    // Then create anchor points based on control points
    // Finally create curve vertices based on every elements
    if (this->_points.size() >= 2)
        this->update();

	this->_nbMaxVertices = this->_bezierCurve.size();

	// Calculate the curve's length (in pixel)
	for (size_t i = 1; i < this->_bezierCurve.size(); i++) {
		sf::Vector2f vec = this->_bezierCurve[i].position - this->_bezierCurve[i - 1].position;

		this->_pixel_length += sqrt(pow(vec.x, 2) + pow(vec.y, 2));
	}

	std::cout << "Cruves vertices number : " << this->_nbMaxVertices << std::endl;
	std::cout << "Curve pixel length : " << this->_pixel_length << std::endl;

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

const sf::Vertex& Bezier::getVertexByTiming(const sf::Time& timing) const
{
	const int index = this->getVertexIndexByTiming(timing);
	
	if (index >= 0 && index < this->_nbMaxVertices)
		return this->_bezierCurve[index];

	return (sf::Vertex());
}

const int Bezier::getVertexIndexByTiming(const sf::Time& timing) const
{
	return (int)floor(this->_nbMaxVertices / this->_duration * (timing - this->getStart()).asSeconds()) - (this->_nbMaxVertices - this->_bezierCurve.size());
}

const sf::Time&	Bezier::getTimingByIndex(const size_t& index) const
{
	return (sf::Time());
}

std::vector<sf::Vertex>&	Bezier::getBezierCurve()
{
    return (this->_bezierCurve);
}

const sf::Time& Bezier::getStart() const
{
	return this->_start;
}

const sf::Time& Bezier::getEnd() const
{
	return sf::seconds(this->_start.asSeconds() + this->_duration);
}

const float&	Bezier::getPixelLength() const
{
	return (this->_pixel_length);
}

const float&	Bezier::getDuration() const
{
	return this->_duration;
}

const sf::Transform&	Bezier::getTransform() const
{
	return this->_transform;
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

void	Bezier::setStart(const sf::Time& start)
{
	this->_start = start;
}

void	Bezier::setDuration(const float& duration)
{
	this->_duration = duration;
}

void	Bezier::setTransform(const sf::Transform& transform)
{
	this->_transform = transform;
}


// METHODS
void Bezier::addPoint(const sf::Vector2f& point)
{
    std::cout << "Add point: [" << point.x << " ; " << point.y << "]" << std::endl;
    this->_points.push_back(point);
}

void Bezier::addTiming(const float& offset)
{
	this->_duration += offset;
}

void Bezier::removeVertex(const std::vector<sf::Vertex>::const_iterator it_points)
{
	this->_bezierCurve.erase(it_points);
}

void Bezier::removeVertexAt(const size_t& index)
{
	this->_bezierCurve.erase(this->_bezierCurve.begin(), this->_bezierCurve.begin() + index);
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
    if (this->_controlPoints.size() >= 2) {
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

    for (size_t i = 0, j = 0; i < this->_anchorPoints.size() - 1; i++, j += 2) {
        std::cout << "Create vertice no " << i << std::endl;
		std::cout << "[" << this->_points[i].x << " ; " << this->_points[i].y << "]" << std::endl;
		this->cubicBezierVertex(
			this->_anchorPoints[i], this->_anchorPoints[i + 1],
			this->_controlPoints[j], this->_controlPoints[j + 1]
		);
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
    const sf::Vector2f& endControl
)
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

	for (auto vertex : ret)
		this->_bezierCurve.push_back(sf::Vertex(vertex, this->_color));
}