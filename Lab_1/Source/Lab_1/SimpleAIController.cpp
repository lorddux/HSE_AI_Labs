// Fill out your copyright notice in the Description page of Project Settings.

#include "Lab_1.h"
#include "SimpleAIController.h"

ASimpleAIController::ASimpleAIController()
{
    bDeliveringOrder = false;
    CurrentOrderNumber = -1;
}

void ASimpleAIController::Tick(float DeltaSeconds)
{
	auto Orders = GetPizzaOrders();
	
	// Take first order.
	auto HouseLocations = GetHouseLocations();

    if (bDeliveringOrder) {
        float Distance = GetDistanceToDestination(CurrentDestination);
        if (Distance > 300.f) {
            SetNewMoveDestination(CurrentDestination);
            return;
        }
        UE_LOG(LogTemp, Warning, TEXT("Trying to deliver order %d, current distance: %1.3f"), CurrentOrderNumber, Distance);
        bool bDeliveredOrder = TryDeliverPizza(CurrentOrderNumber);
        if (bDeliveredOrder) {
            UE_LOG(LogTemp, Warning, TEXT("Delivered order %d"), CurrentOrderNumber);
            bDeliveringOrder = false;
            CurrentOrderNumber = -1;
        } else {
			//CrucialTimeDiffer = 10;
			//for (int i = 0; i < Orders.Num(); ++i) {
			//	TimeDiffer = GetHouseTimeLeft(Orders[i].HouseNumber) - GetDistanceToDestination(HouseLocations[Orders[i].HouseNumber]) / 600 - 1;
			//	if (TimeDiffer < 0) {
			//		CrucialTimeDiffer = TimeDiffer;
			//		CurrentOrderNumber = Orders[i].OrderNumber;
			//		CurrentDestination = HouseLocations[Orders[i].HouseNumber];
			//		SetNewMoveDestination(CurrentDestination);
			//		return;
			//	}
			//}
			SetNewMoveDestination(CurrentDestination);
        }
        return;
    }

	if (Orders.Num() == 0) {
		// No orders to serve.
		return;
	}

    int closestOrder = 0;
    float closestDistance = GetDistanceToDestination(HouseLocations[Orders[0].HouseNumber]) + GetHouseTimeLeft(Orders[0].HouseNumber) * 50;
    for (int i = 0; i < Orders.Num(); ++i) {
        float currentDistance = GetDistanceToDestination(HouseLocations[Orders[i].HouseNumber]) + GetHouseTimeLeft(Orders[i].HouseNumber) * 50;
        if (currentDistance < closestDistance) {
            closestDistance = currentDistance;
            closestOrder = i;
			//SetNewMoveDestination(HouseLocations[Orders[i].HouseNumber]);
        }
		if (GetDistanceToDestination(HouseLocations[Orders[i].HouseNumber]) / 600 > GetHouseTimeLeft(Orders[i].HouseNumber) - 1.5) {
			closestDistance = 0;
			closestOrder = i;
		}
    }
    auto Order = Orders[closestOrder];

    int PizzaAmount = GetPizzaAmount();
    if (PizzaAmount == 0) {
        bool bGrabbedPizza = TryGrabPizza();
        // Failed to retrieve pizza, need to get closer to the bakery.
        if (!bGrabbedPizza) {
            return;
        }
    }

    auto HouseLocation = HouseLocations[Order.HouseNumber];
    bDeliveringOrder = true;
    CurrentOrderNumber = Order.OrderNumber;
    CurrentDestination = HouseLocation;
    SetNewMoveDestination(HouseLocation);
    UE_LOG(LogTemp, Warning, TEXT("Took new order %d to house %d"), Order.OrderNumber, Order.HouseNumber);
}

