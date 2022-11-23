#pragma once

#include "Subsystem.h"

#include "File.h"
#include "INIFile.h"
#include "Image.h"
#include "Model.h"
#include "Material.h"

#include <memory>

namespace JFF
{
	class IO : public Subsystem
	{
	public:
		// Ctor & Dtor
		IO() {}
		virtual ~IO() {}

		// Copy ctor and copy assignment
		IO(const IO& other) = delete;
		IO& operator=(const IO& other) = delete;

		// Move ctor and assignment
		IO(IO&& other) = delete;
		IO operator=(IO&& other) = delete;

		// IO interface
		[[nodiscard]] virtual std::shared_ptr<File> loadRawFile(const char* filename) const = 0;
		[[nodiscard]] virtual std::shared_ptr<INIFile> loadINIFile(const char* filename) const = 0;
		[[nodiscard]] virtual std::shared_ptr<Image> loadImage(const char* filename, bool flipVertically = true) const = 0;
		[[nodiscard]] virtual std::shared_ptr<Model> loadModel(const char* assetFilepath,
			const std::weak_ptr<JFF::GameObject>& parentGameObject = std::weak_ptr<JFF::GameObject>()) const = 0;
	};
}