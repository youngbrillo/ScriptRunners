#pragma once
#include "Manager.h"
#include <vector>
#include <map>
#include <set>
#include <string>
#include "AnimationHelper.h"
#include <memory>
class AnimationStateManager :  public Manager
{
public:
	AnimationStateManager();
	~AnimationStateManager();
	virtual void init()override;
	virtual void clear()override;
	virtual void Update(const float& dt) override;
	virtual void CameraDraw(Camera2D& camera) override;
	virtual void CanvasDraw() override;
	virtual void innerdebug() override;
	void Populate(unsigned int textureId, std::string textureName, std::string path);

	Animation::StateMachine& GetAnimator(unsigned int texture_id);
	static std::shared_ptr<AnimationStateManager> Instance();
	static Manager Register();
private:


	std::map<unsigned int, Animation::StateMachine> stateMachines;		//for global use
	std::map<unsigned int, Animation::Resource> resources;	//for internal use
	std::map<std::string, unsigned int> animatorLookup;				
	std::map<unsigned int, std::string> reverse_animatorLookup;
	std::set<std::string> aliasDB;
	bool active = false;
};
