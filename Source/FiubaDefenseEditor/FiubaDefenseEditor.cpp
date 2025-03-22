#include "FiubaDefenseEditor.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "FDDamageProfileSettings.h"
#include "UnrealEdGlobals.h"
#include "ComponentVisualizers/FFDTowerVisualizerComponent.h"
#include "DamageProfile/FDDamageProfileDetails.h"
#include "Editor/UnrealEdEngine.h"
#include "Tower/FDTowerGridComponent.h"


IMPLEMENT_GAME_MODULE(FFiubaDefenseEditorModule, CardDefenseEditor);

DEFINE_LOG_CATEGORY(FiubaDefenseEditor)
 
#define LOCTEXT_NAMESPACE "FiubaDefenseEditor"
 
void FFiubaDefenseEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	PropertyModule.RegisterCustomClassLayout("FDDamageProfileSettings", FOnGetDetailCustomizationInstance::CreateStatic(&FFDDamageProfileDetails::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();

	RegisterComponentVisualizers();
}

void FFiubaDefenseEditorModule::ShutdownModule()
{
	UnregisterComponentVisualizers();
}

void FFiubaDefenseEditorModule::RegisterComponentVisualizers()
{
	if(GUnrealEd)
	{
		TSharedPtr<FComponentVisualizer> TowerVisualizerComponent = MakeShareable(new FFDTowerVisualizerComponent());
		GUnrealEd->RegisterComponentVisualizer(UFDTowerGridComponent::StaticClass()->GetFName(), TowerVisualizerComponent);
		TowerVisualizerComponent->OnRegister();
	}
}

void FFiubaDefenseEditorModule::UnregisterComponentVisualizers()
{
	if(GUnrealEd)
	{
		GUnrealEd->UnregisterComponentVisualizer(UFDTowerGridComponent::StaticClass()->GetFName());
	}
}

#undef LOCTEXT_NAMESPACE
