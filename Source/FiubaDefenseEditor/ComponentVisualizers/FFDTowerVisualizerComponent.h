#pragma once

#include "ComponentVisualizer.h"
#include "CoreMinimal.h"

class FFDTowerVisualizerComponent : public FComponentVisualizer
{
public:

	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
};



