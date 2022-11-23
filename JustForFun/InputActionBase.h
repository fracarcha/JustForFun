#pragma once

#include <string>

namespace JFF
{
	class InputActionBase
	{
	public:
		// Ctor & Dtor
		InputActionBase() {}
		virtual ~InputActionBase() {}

		// Copy ctor and copy assignment
		InputActionBase(const InputActionBase& other) = delete;
		InputActionBase& operator=(const InputActionBase& other) = delete;

		// Move ctor and assignment
		InputActionBase(InputActionBase&& other) = delete;
		InputActionBase operator=(InputActionBase&& other) = delete;

		// -------------------------------- INPUT ACTION BASE INTERFACE -------------------------------- //

		virtual std::string getName() const = 0;

		// Calls all listeners subscribed to this action and send them the updated value
		virtual void execute() = 0;
	};
}