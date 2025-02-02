// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemInstance.h"
#include "InventoryComponent.h"

void UItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

//
// TODO: Need to create a new AActor subclass and use it for all actors spawned by an item instance.
// This can help with many things, like controlling the replication & lifetime of spawned actors (e.g., setting their 
// outer object to owner or the persistent level)
// Which could be useful if a character summons a minion actor, we might want to destroy the minion actor after the player dies.
//
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

	/** Spawn the actor at the owner actor's location (if available) */
	FTransform SpawnLocation = OwnerActor ? OwnerActor->GetTransform() : FTransform::Identity;

	/** Get the UClass for this item's actor */
	TSubclassOf<AActor> ItemActorClass = Data->GetItemActorClass();

	if (ItemActorClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UItemInstance::SpawnItemActor failed, ItemActorClass was null"));
		return;
	}

	/** Spawn the ItemActor & set it to replicate */
	AActor* ItemActor = World->SpawnActor<AActor>(ItemActorClass, SpawnLocation, SpawnParams);
	ItemActor->SetReplicates(true);

	// TODO: Should we set the Outer for the ItemActor to the character that owns the ItemInstance?

	UE_LOG(LogTemp, Log, TEXT("ItemActor's owner: %s, ItemActor's outer: %s"), *ItemActor->Owner->GetName(), *ItemActor->GetOuter()->GetName());
}
