// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemInstance.h"
#include "InventoryComponent.h"

void UItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UItemInstance::SpawnItemActor()
{
	UInventoryComponent* Inventory = Cast<UInventoryComponent>(GetOuter());
	check(Inventory);

	UWorld* World = Inventory->GetWorld();
	check(World);

	if (Inventory->GetOwnerRole() != ENetRole::ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemInstance::SpawnItemActor called on client-side (must be on authority)"));
		return;
	}

	/** Use this struct to customize spawning behavior */
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerActor;

	FTransform SpawnLocation = OwnerActor ? OwnerActor->GetTransform() : FTransform::Identity;

	/** Get the UClass for this item's actor */
	TSubclassOf<AActor> ItemActorClass = Data->GetItemActorClass();

	if (ItemActorClass != nullptr)
	{
		World->SpawnActor<AActor>(ItemActorClass, SpawnLocation, SpawnParams);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UItemInstance::SpawnItemActor failed, ItemActorClass was null"));
		return;
	}
}
