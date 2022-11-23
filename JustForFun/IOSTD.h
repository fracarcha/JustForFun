#pragma once

#include "IO.h"

namespace JFF
{
	// Standard implementation of Input/Output subsystem
	class IOSTD : public IO
	{
	public:
		// Ctor & Dtor
		IOSTD();
		virtual ~IOSTD();

		// Copy ctor and copy assignment
		IOSTD(const IOSTD& other) = delete;
		IOSTD& operator=(const IOSTD& other) = delete;

		// Move ctor and assignment
		IOSTD(IOSTD&& other) = delete;
		IOSTD operator=(IOSTD&& other) = delete;

		// Subsystem impl
		virtual void load() override;
		virtual void postLoad(Engine* engine) override;
		virtual UnloadOrder getUnloadOrder() const override;

		// IO interface
		[[nodiscard]] virtual std::shared_ptr<File> loadRawFile(const char* filename) const override;
		[[nodiscard]] virtual std::shared_ptr<INIFile> loadINIFile(const char* filename) const override;
		[[nodiscard]] virtual std::shared_ptr<Image> loadImage(const char* filename, bool flipVertically = true) const override;
		[[nodiscard]] virtual std::shared_ptr<Model> loadModel(const char* assetFilepath,
			const std::weak_ptr<JFF::GameObject>& parentGameObject = std::weak_ptr<JFF::GameObject>()) const override;
	
	protected:
		Engine* engine;
	};
}