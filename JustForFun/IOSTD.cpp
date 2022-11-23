#include "IOSTD.h"

#include "Log.h"

JFF::IOSTD::IOSTD() :
	engine()
{
	JFF_LOG_INFO_LOW_PRIORITY("Ctor subsystem: IOSTD")
}

JFF::IOSTD::~IOSTD()
{
	JFF_LOG_IMPORTANT("Dtor subsystem: IOSTD")
}

void JFF::IOSTD::load()
{
	JFF_LOG_IMPORTANT("Loading subsystem: IOSTD")
}

void JFF::IOSTD::postLoad(Engine* engine)
{
	JFF_LOG_IMPORTANT("Post-loading subsystem: IOSTD")
	this->engine = engine;
}

JFF::Subsystem::UnloadOrder JFF::IOSTD::getUnloadOrder() const
{
	return UnloadOrder::IO;
}

extern std::shared_ptr<JFF::File> createFile(const char* filepath);
extern std::shared_ptr<JFF::INIFile> createINIFile(const char* filepath);
extern std::shared_ptr<JFF::Image> createImage(JFF::Engine* const engine, const char* filepath, bool flipVertically = true);

extern std::shared_ptr<JFF::Model> createModel(const char* assetFilepath, JFF::Engine* const engine);
extern std::shared_ptr<JFF::Model> createModel(const char* assetFilepath, JFF::Engine* const engine, const std::weak_ptr<JFF::GameObject>& parentGameObject);

std::shared_ptr<JFF::File> JFF::IOSTD::loadRawFile(const char* filename) const
{
	return createFile(filename);
}

std::shared_ptr<JFF::INIFile> JFF::IOSTD::loadINIFile(const char* filename) const
{
	return createINIFile(filename);
}

std::shared_ptr<JFF::Image> JFF::IOSTD::loadImage(const char* filename, bool flipVertically) const
{
	return createImage(engine, filename, flipVertically);
}

std::shared_ptr<JFF::Model> JFF::IOSTD::loadModel(const char* assetFilepath, const std::weak_ptr<JFF::GameObject>& parentGameObject) const
{
	if (parentGameObject.expired())
		return createModel(assetFilepath, engine);
	else
		return createModel(assetFilepath, engine, parentGameObject);
}