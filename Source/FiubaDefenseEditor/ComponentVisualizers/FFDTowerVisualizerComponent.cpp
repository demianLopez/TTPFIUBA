#include "FFDTowerVisualizerComponent.h"

#include "Tower/FDTower.h"
#include "Tower/FDTowerGridComponent.h"
#include "Tower/FDTowerGridComponentTypes.h"

void FFDTowerVisualizerComponent::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	const UFDTowerGridComponent* TowerGridComponent = Cast<UFDTowerGridComponent>(Component);

	if(!IsValid(TowerGridComponent))
		return;	

	for (const TSharedPtr<FFDTowerGridTile> GridTile : TowerGridComponent->GetTowerGridTiles())
	{
		FVector HalfTileSize = GridTile->GetTileWorldSize() * 0.5f;
		HalfTileSize.Z = 10.0f;

		FVector GridTileWorldLocation = GridTile->GetTileWorldLocation();
		
		const FVector& UpperGridBound = GridTileWorldLocation - HalfTileSize;
		const FVector& LowerGridBound = GridTileWorldLocation + HalfTileSize;
		
		FBox Box;
		Box.Min = LowerGridBound;
		Box.Max = UpperGridBound;
		
		DrawWireBox(PDI, Box, FColor::Red, 1, 1.0f);
	}
}
