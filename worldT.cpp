#include "worldT.h"

void worldT(WorldTransform& worldTransform) {
	worldTransform.matWorld_ = MakeAfineMatrix(worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_);
	worldTransform.TransferMatrix();
}
