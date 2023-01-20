/*
* ---------------------------------------------------------------------------
* JustForFun game engine
* ---------------------------------------------------------------------------
*
* Copyright (c) 2022-2023, Francisco José Carmona.
*
* All Rights Reserved.
*/

#include "InputProcessorNormalizer.h"

template<typename _Ret>
inline JFF::InputProcessorNormalizer<_Ret>::InputProcessorNormalizer(Engine* const engine) :
	InputProcessor<_Ret>(engine),
	engine(engine)
{
	JFF_LOG_INFO("Ctor InputProcessorNormalizer")
}

template<typename _Ret>
inline JFF::InputProcessorNormalizer<_Ret>::~InputProcessorNormalizer()
{
	JFF_LOG_INFO("Dtor InputProcessorNormalizer")
}

template<typename _Ret>
inline _Ret JFF::InputProcessorNormalizer<_Ret>::process(const _Ret& inputValue) const
{
	JFF_LOG_ERROR("This processor is axes compatible only")
	return inputValue;
}

template<>
inline JFF::Vec2 JFF::InputProcessorNormalizer<JFF::Vec2>::process(const JFF::Vec2& inputValue) const
{
	return engine->math.lock()->normalize(inputValue);
}

