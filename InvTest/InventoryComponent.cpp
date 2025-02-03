// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{

	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

const UItemInstance* UInventoryComponent::CreateItemInInventory(TSubclassOf<UItemInstance> ItemClass, UItemData* ItemData)
{
	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::CreateItemInInventory called on %s"), GetOwner()->HasAuthority() ? TEXT("Server") : TEXT("Client"));

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
	// TODO: We will need to manually remove this whenever the item is removed from the inventroy
	AddReplicatedSubObject(Item);

	Items.Add(Item);


	return Item;
}

void UInventoryComponent::ServerCreateItemInInventory_Implementation(TSubclassOf<UItemInstance> ItemClass, UItemData* ItemData)
{
	CreateItemInInventory(ItemClass, ItemData);
}


void UInventoryComponent::OnRep_Items()
{
	UE_LOG(LogTemp, Log, TEXT("OnRep_Items!"));
	for (const auto& Item : Items)
	{
		UE_LOG(LogTemp, Log, TEXT("\t - Item: %s"), Item ? *Item->GetName() : TEXT("nullptr"));
	}
}

