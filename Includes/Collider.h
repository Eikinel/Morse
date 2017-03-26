#pragma once

/* A collider is a shape (rectangle, sphere, etc.) contained by an entity.
** When a collision occure, effects will vary depending on what the entity in cause should do. */

#include "Constants.h"

enum				eColliderType
{
	HEAD,
	BODY,
	ARM,
	HAND,
	LEG,
	FOOT,
	CASE,
	OTHER
};

template <typename T>
class					ICollider
{
public:
	T&					getShape()
	{
		return (this->_shape);
	}

	const eColliderType	getType() const
	{
		return (this->_type);
	}

protected:
	T				_shape;
	eColliderType	_type;
};

class				BoxCollider : public ICollider<sf::RectangleShape>
{
public:
	BoxCollider(const sf::RectangleShape& rect, const eColliderType type = OTHER)
	{
		std::cout << "Add box collider at position [" << rect.getGlobalBounds().left << " ; " << rect.getGlobalBounds().top << "] and size [" << rect.getGlobalBounds().width << " ; " << rect.getGlobalBounds().height << "]." << std::endl;
		this->_shape = rect;
		this->_type = type;
	};

	BoxCollider(const sf::FloatRect& rect, const eColliderType type = OTHER)
	{
		std::cout << "Add box collider at position [" << rect.left << " ; " << rect.top << "] and size [" << rect.width << " ; " << rect.height << "]." << std::endl;
		this->_shape.setPosition(sf::Vector2f(rect.left, rect.top));
		this->_shape.setSize(sf::Vector2f(rect.width, rect.height));
		this->_type = type;
	};

	BoxCollider(float x, float y, int width, int height, const eColliderType type = OTHER)
	{
		std::cout << "Add box collider at position [" << x << " ; " << y << "] and size [" << width << " ; " << height << "]." << std::endl;
		this->_shape.setPosition(sf::Vector2f(x, y));
		this->_shape.setSize(sf::Vector2f(width, height));
		this->_type = type;
	}

	template <typename U> BoxCollider(const sf::Vector2<U>& pos, const sf::Vector2<U>& size, const eColliderType type = OTHER)
	{
		std::cout << "Add box collider at position [" << pos.x << " ; " << pos.y << "] and size [" << size.x << " ; " << size.y << "]." << std::endl;
		this->_shape.setPosition(pos);
		this->_shape.setSize(size);
		this->_type = type;
	}

	virtual ~BoxCollider() {}
};