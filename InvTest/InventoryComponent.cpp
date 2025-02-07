// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"

UInventoryComponent::UInventoryComponent()
{
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, Items);
}


const UItemInstance* UInventoryComponent::CreateItemInInventory(TSubclassOf<UItemInstance> ItemClass, UItemData* ItemData)
{
	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::CreateItemInInventory called with role: %s"), *UEnum::GetValueAsString(GetOwnerRole()));

	// If called on client, make a server rpc to ServerCreateItemInInventory
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Log, TEXT("Forwarding CreateItemInInventory call to ServerCreateItemInInventory"));
		ServerCreateItemInInventory(ItemClass, ItemData);
		return nullptr;
	}

	FItemInstanceInitializer ItemInitializer;
	ItemInitializer.Outer = this;
	ItemInitializer.OwnerActor = GetOwner();
	ItemInitializer.ItemClass = ItemClass;
	ItemInitializer.ItemData = ItemData;

	UItemInstance* Item = UItemInstance::CreateItemInstance(ItemInitializer);

	/** Important: Add item to the replicated subobjects list, otherwise it wont be replicated*/
	/** The item Items array itself will replicate, but the UItemInstance* inside of them will be nullptr.*/
	// TODO: We will need to manually remove items from this whenever the item is removed from the inventroy
	AddReplicatedSubObject(Item);

	Items.Add(Item);

	return Item;
}

void UInventoryComponent::ServerCreateItemInInventory_Implementation(TSubclassOf<UItemInstance> ItemClass, UItemData* ItemData)
{
	CreateItemInInventory(ItemClass, ItemData);
}

void UInventoryComponent::ServerSpawnItemActor_Implementation(UItemInstance* InItemInstance)
{
	if (IsItemActorSpawned(InItemInstance))
	{
		UE_LOG(LogTemp, Warning, TEXT("Tried to spawn an ItemActor, but the instance's ItemActor is already spawned"));
		return;
	}

	AActor* SpawnedItemActor = InItemInstance->TrySpawnItemActor();

	if (!SpawnedItemActor)
	{
		return;
	}

	SpawnedItemActors.Add(InItemInstance);

}


void UInventoryComponent::ServerDestroyItemActor_Implementation(UItemInstance* InItemInstance)
{
	if (!IsItemActorSpawned(InItemInstance))
	{
		UE_LOG(LogTemp, Warning, TEXT("Tried to destroy an ItemActor, but the instance had no ItemActor spawned"));
		return;
	}

	InItemInstance->TryDestroyItemActor();

	SpawnedItemActors.Remove(InItemInstance);
}


bool UInventoryComponent::IsItemActorSpawned(UItemInstance* InItemInstance) const
{
	if (SpawnedItemActors.Find(InItemInstance))
	{
		return true;
	}
	return false;
}

void UInventoryComponent::OnRep_Items()
{
	UE_LOG(LogTemp, Log, TEXT("OnRep_Items!"));
	for (const auto& Item : Items)
	{
		UE_LOG(LogTemp, Log, TEXT("\t - Item: %s"), Item ? *Item->GetName() : TEXT("nullptr"));
	}
}

