#include "cashgenUE.h"
#include "ZoneManager.h"

AZoneManager::AZoneManager()
{
	PrimaryActorTick.bCanEverTick = false;
	USphereComponent* SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootComponent = SphereComponent;
	MyProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
}

void AZoneManager::SetupZone(Point aOffset, int32 aX, int32 aY, float aUnitSize, UMaterial* aMaterial, float aFloor, float aPersistence, float aFrequency, float aAmplitude, int32 aOctaves, int32 aRandomseed)
{
	MyMaterial = aMaterial;
	gridSize = aUnitSize;
	worldGen = new WorldGenerator();
	worldGen->InitialiseTerrainGrid(&MyZoneData, &MyHeightMap, aOffset, aX, aY, aFloor, aPersistence, aFrequency, aAmplitude, aOctaves, aRandomseed);
	LoadTerrainGridAndGenerateMesh();
}

void AZoneManager::LoadTerrainGridAndGenerateMesh()
{
	for (int32 x = 0; x < MyZoneData.Num() -2; ++x)
	{
		for (int32 y = 0; y < MyZoneData[x].blocks.Num() -2; ++y)
		{
			AddQuad(&MyZoneData[x+1].blocks[y+1], x, y);
		}
	}

	CreateSection();
}

FVector AZoneManager::CalcSurfaceNormalForTriangle(const int32 aStartTriangle)
{
	FVector v1 = MyVertices[aStartTriangle];
	FVector v2 = MyVertices[aStartTriangle + 1];
	FVector v3 = MyVertices[aStartTriangle + 2];

	FVector U = v2 - v1;
	FVector V = v3 - v1;

	return FVector::CrossProduct(V, U).GetSafeNormal();
}

void AZoneManager::AddQuad(ZoneBlock* block, int32 aX, int32 aY)
{
	int32 numTriangles = MyVertices.Num();

	MyVertices.Add(FVector(aX * gridSize - (gridSize*0.5), (aY * gridSize) - (gridSize*0.5), block->bottomLeftCorner.height));//BL
	MyVertices.Add(FVector(aX * gridSize - (gridSize*0.5), (aY * gridSize) + (gridSize*0.5), block->topLeftCorner.height));//TL
	MyVertices.Add(FVector((aX * gridSize) + (gridSize * 0.5), (aY * gridSize) - (gridSize*0.5), block->bottomRightCorner.height));//BR

	MyVertices.Add(FVector((aX * gridSize) - (gridSize*0.5), (aY * gridSize) + (gridSize*0.5), block->topLeftCorner.height));//TL
	MyVertices.Add(FVector((aX * gridSize) + (gridSize*0.5), (aY * gridSize) + (gridSize*0.5), block->topRightCorner.height));//TR
	MyVertices.Add(FVector((aX * gridSize) + (gridSize*0.5), (aY * gridSize) - (gridSize*0.5), block->bottomRightCorner.height));//BR
	
	MyTriangles.Add(numTriangles);
	MyTriangles.Add(numTriangles + 1);
	MyTriangles.Add(numTriangles + 2);
	MyTriangles.Add(numTriangles + 3);
	MyTriangles.Add(numTriangles + 4);
	MyTriangles.Add(numTriangles + 5);

	FVector t1Normal = CalcSurfaceNormalForTriangle(numTriangles);
	FVector t2Normal = CalcSurfaceNormalForTriangle(numTriangles + 3);

	MyNormals.Add(t1Normal);
	MyNormals.Add(t1Normal);
	MyNormals.Add(t1Normal);
	MyNormals.Add(t2Normal);
	MyNormals.Add(t2Normal);
	MyNormals.Add(t2Normal);

	MyUV0.Add(FVector2D(0, 0));
	MyUV0.Add(FVector2D(0, 1));
	MyUV0.Add(FVector2D(1, 0));
	MyUV0.Add(FVector2D(0, 1));
	MyUV0.Add(FVector2D(1, 1));
	MyUV0.Add(FVector2D(1, 0));

	MyVertexColors.Add(block->Color);
	MyVertexColors.Add(block->Color);
	MyVertexColors.Add(block->Color);
	MyVertexColors.Add(block->Color);
	MyVertexColors.Add(block->Color);
	MyVertexColors.Add(block->Color);
}

void AZoneManager::CreateSection()
{
	MyProcMesh->SetMaterial(0, MyMaterial);
	MyProcMesh->CreateMeshSection(0, MyVertices, MyTriangles, MyNormals, MyUV0, MyVertexColors, MyTangents, true);
	MyProcMesh->AttachTo(RootComponent);
}

AZoneManager::~AZoneManager()
{
	delete worldGen;
}

// Called when the game starts or when spawned
void AZoneManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AZoneManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}