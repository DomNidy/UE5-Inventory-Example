// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemInstance.h"
#include "InventoryComponent.h"

void UItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UItemInstance, ItemActor);
}

UItemInstance* UItemInstance::CreateItemInstance(const FItemInstanceInitializer& ItemInitializer)
{
	UE_LOG(LogTemp, Log, TEXT("UItemInstance::CreateItemInstance called on %s"), ItemInitializer.OwnerActor->HasAuthority() ? TEXT("Server") : TEXT("Client"));
	checkf(ItemInitializer.OwnerActor->HasAuthority(), TEXT("UItemInstance::CreateItemInstance was called on a client, this should only be called on the server."));
	checkf(ItemInitializer.ItemData && ItemInitializer.ItemClass && ItemInitializer.Outer && ItemInitializer.OwnerActor, TEXT("Either ItemClass, ItemData, Outer, or OwnerActor was null. ItemClass defined?: %s, ItemData defined?: %s, Outer defined?: %s, OwnerActor defined?: %s"),
		ItemInitializer.ItemClass ? TEXT("True") : TEXT("False"),
		ItemInitializer.ItemData ? TEXT("True") : TEXT("False"),
		ItemInitializer.Outer ? TEXT("True") : TEXT("False"),
		ItemInitializer.OwnerActor ? TEXT("True") : TEXT("False"));

	UItemInstance* Item = NewObject<UItemInstance>(ItemInitializer.Outer, ItemInitializer.ItemClass);
	Item->Data = ItemInitializer.ItemData;
	Item->OwnerActor = ItemInitializer.OwnerActor;

	return Item;
}

bool UItemInstance::IsItemActorSpawned() const
{
	return IsValid(ItemActor);
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
	AActor* SpawnedItemActor = World->SpawnActor<AActor>(ItemActorClass, SpawnLocation, SpawnParams);

	/** Bind to item actor lifecycle delegates */
	SpawnedItemActor->OnDestroyed.AddDynamic(this, &UItemInstance::HandleItemActorDestroyed);

	ItemActor = SpawnedItemActor;
	SpawnedItemActor->SetReplicates(true);

	UE_LOG(LogTemp, Log, TEXT("Spawned an ItemActor: ItemActor's owner: %s, ItemActor's outer: %s"), *SpawnedItemActor->Owner->GetName(), *SpawnedItemActor->GetOuter()->GetName());
}

void UItemInstance::DestroyItemActor()
{
	if (IsValid(ItemActor))
	{
		ItemActor->Destroy();
	}
}

void UItemInstance::HandleItemActorDestroyed(AActor* InActor)
{
	UE_LOG(LogTemp, Log, TEXT("UItemInstance::HandleItemActorDestroyed: %s was destroyed!"), *InActor->GetName());
	ItemActor = nullptr;
}
