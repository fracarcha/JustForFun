/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "InputAction.h"

#include "Log.h"
#include <algorithm>

template<typename _Ret>
inline JFF::InputAction<_Ret>::InputAction(const std::string& name, Engine* const engine) :
	engine(engine),
	name(name),
	lastActionValue(),
	lastActionHardware(Input::Hardware::KEYBOARD),
	listeners()
{
	JFF_LOG_INFO("Ctor InputAction")
}

template<typename _Ret>
inline JFF::InputAction<_Ret>::~InputAction()
{
	JFF_LOG_INFO("Dtor InputAction")
}

template<typename _Ret>
inline std::string JFF::InputAction<_Ret>::getName() const
{
	return name;
}

template<typename _Ret>
inline void JFF::InputAction<_Ret>::execute()
{
	std::for_each(listeners.begin(), listeners.end(), [this](const auto& pair)
		{
			pair.second(lastActionValue, lastActionHardware);
		});
}

template<typename _Ret>
inline void JFF::InputAction<_Ret>::setActionValue(const _Ret& value, Input::Hardware hw)
{
	// Change the input value and hardware
	lastActionValue = value;
	lastActionHardware = hw;

	// Warn Input that this action has received an input and must be executed
	engine->input.lock()->markActiveAction(name);
}

template<typename _Ret>
inline void JFF::InputAction<_Ret>::addListener(const InputComponent* const inputComp, const std::function<void(const _Ret&, Input::Hardware hw)>& listener)
{
	// Check if listener is already included
	auto iter = listeners.find(inputComp);
	if (iter != listeners.end())
	{
		JFF_LOG_WARNING("Input listener is already included in this action. The old one will be removed")
	}

	// Add the listener
	listeners[inputComp] = listener;
}

template<typename _Ret>
inline void JFF::InputAction<_Ret>::removeListener(const InputComponent* const inputComp)
{
	size_t numElementsRemoved = listeners.erase(inputComp);
	if (numElementsRemoved <= 0)
	{
		JFF_LOG_WARNING("Attempting to remove a non existent InputComponent listener. Operation aborted")
	}
}