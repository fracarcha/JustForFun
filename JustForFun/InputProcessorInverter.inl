/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "InputProcessorInverter.h"

template<typename _Ret>
inline JFF::InputProcessorInverter<_Ret>::InputProcessorInverter(Engine* const engine, bool invertX, bool invertY) : 
	InputProcessor<_Ret>(engine),
	engine(engine),
	invertX(invertX),
	invertY(invertY)
{
	JFF_LOG_INFO("Ctor InputProcessorInverter")
}

template<typename _Ret>
inline JFF::InputProcessorInverter<_Ret>::~InputProcessorInverter()
{
	JFF_LOG_INFO("Dtor InputProcessorInverter")
}

template<typename _Ret>
inline _Ret JFF::InputProcessorInverter<_Ret>::process(const _Ret& inputValue) const
{
	JFF_LOG_ERROR("This processor is axes compatible only")
	return inputValue;
}

template<>
inline JFF::Vec2 JFF::InputProcessorInverter<JFF::Vec2>::process(const JFF::Vec2& inputValue) const
{
	return Vec2(invertX ? -inputValue.x : inputValue.x, invertY ? -inputValue.y : inputValue.y);
}