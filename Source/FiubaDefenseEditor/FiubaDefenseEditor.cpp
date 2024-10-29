#include "FiubaDefenseEditor.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "FDDamageProfileSettings.h"
#include "DamageProfile/FDDamageProfileDetails.h"


IMPLEMENT_GAME_MODULE(FFiubaDefenseEditorModule, CardDefenseEditor);

DEFINE_LOG_CATEGORY(FiubaDefenseEditor)
 
#define LOCTEXT_NAMESPACE "FiubaDefenseEditor"
 
void FFiubaDefenseEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	PropertyModule.RegisterCustomClassLayout("FDDamageProfileSettings", FOnGetDetailCustomizationInstance::CreateStatic(&FFDDamageProfileDetails::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();	
}

void FFiubaDefenseEditorModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
