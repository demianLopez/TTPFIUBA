#pragma once
 
#include "Core.h"
#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(FiubaDefenseEditor, All, All)
 
class FFiubaDefenseEditorModule: public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

protected:

	void RegisterComponentVisualizers();
	void UnregisterComponentVisualizers();
};