#include "MyComponent.h"

#include "Log.h"

JFF::MyComponent::MyComponent(GameObject* const gameObject, const char* name, bool initiallyEnabled) :
	Component(gameObject, name, initiallyEnabled)
{
	JFF_LOG_INFO("Ctor MyComponent")
}

JFF::MyComponent::~MyComponent()
{
	JFF_LOG_INFO("Dtor MyComponent")
}

void JFF::MyComponent::onStart()
{
	// TODO: Enter your starting code here
}
