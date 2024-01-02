#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include <assert.h>
#include "Manager.h"



class GlobalManager
{
private:
	GlobalManager();
	~GlobalManager();
public:
	static GlobalManager* Get();
	static void Begin();
	static void End();
	static void Set();
	static void Reset();
	static void Update(const float& dt) { Get()->_update(dt); }
	static void Draw(Camera2D& camera) { Get()->_draw(camera); };
	static void CanvasDraw() { Get()->_canvasDraw(); };
	static void Debug(const char* title) { Get()->debug(title); };

private:
	void _update(const float& dt);
	void _draw(Camera2D& cam);
	void _canvasDraw();
	void debug(const char* title);

private:
	static GlobalManager* Instance;
	std::unordered_map<const char*, std::shared_ptr<Manager>> mManagers{};
	template<typename T>
	std::shared_ptr<T>  registerManager();

	template<typename T>
	std::shared_ptr<T>  getManager();
public:
	template<typename T>
	static std::shared_ptr<T>  Register_Manager() { return Get()->registerManager<T>(); }

	template<typename T>
	static std::shared_ptr<T>  Get_Manager() { return Get()->getManager<T>(); }

};

template<typename T>
inline std::shared_ptr<T>  GlobalManager::registerManager()
{
	const char* typeName = typeid(T).name();
	assert(mManagers.find(typeName) == mManagers.end() && "Registering Manager more than once.");
	auto i = std::make_shared<T>();
	mManagers.insert({ typeName, i });
	return i;
}

template<typename T>
inline std::shared_ptr<T> GlobalManager::getManager()
{
	const char* typeName = typeid(T).name();
	assert(mManagers.find(typeName) != mManagers.end() && "Attempting to get unregistered Manager .");
	std::shared_ptr<T> access = std::dynamic_pointer_cast<T>(mManagers.at(typeName));
	return access;
}
