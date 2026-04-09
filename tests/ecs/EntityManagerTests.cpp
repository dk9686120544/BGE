#include "ecs/EntityManager.h"
#include "tests/TestFramework.h"

using namespace bge;

BGE_TEST(EntityManagerTests, DefaultStateIsEmpty)
{
    EntityManager manager;

    BGE_TEST_EQ(manager.Capacity(), 0U);
    BGE_TEST_EQ(manager.AliveCount(), 0U);
    BGE_TEST_TRUE(manager.DeferredDestroyQueue().empty());
}

BGE_TEST(EntityManagerTests, CreateReturnsValidEntity)
{
    EntityManager manager;

    const Entity entity = manager.Create();

    BGE_TEST_TRUE(entity.IsValid());
    BGE_TEST_TRUE(manager.IsAlive(entity));
    BGE_TEST_EQ(entity.index, 0U);
    BGE_TEST_EQ(entity.generation, 0U);
    BGE_TEST_EQ(manager.AliveCount(), 1U);
    BGE_TEST_TRUE(manager.Capacity() >= 1U);
}

BGE_TEST(EntityManagerTests, CreateMultipleEntitiesUsesSequentialIndicesInitially)
{
    EntityManager manager;

    const Entity a = manager.Create();
    const Entity b = manager.Create();
    const Entity c = manager.Create();

    BGE_TEST_EQ(a.index, 0U);
    BGE_TEST_EQ(b.index, 1U);
    BGE_TEST_EQ(c.index, 2U);

    BGE_TEST_TRUE(manager.IsAlive(a));
    BGE_TEST_TRUE(manager.IsAlive(b));
    BGE_TEST_TRUE(manager.IsAlive(c));

    BGE_TEST_EQ(manager.AliveCount(), 3U);
}

BGE_TEST(EntityManagerTests, InvalidEntityIsNotAlive)
{
    EntityManager manager;

    const Entity invalid = Entity::Invalid();

    BGE_TEST_FALSE(manager.IsAlive(invalid));
}

BGE_TEST(EntityManagerTests, MarkForDestroyDoesNotDestroyImmediately)
{
    EntityManager manager;

    const Entity entity = manager.Create();
    manager.MarkForDestroy(entity);

    BGE_TEST_TRUE(manager.IsAlive(entity));
    BGE_TEST_EQ(manager.AliveCount(), 1U);
    BGE_TEST_EQ(manager.DeferredDestroyQueue().size(), static_cast<usize>(1));
}

BGE_TEST(EntityManagerTests, ProcessDeferredDestroyDestroysEntity)
{
    EntityManager manager;

    const Entity entity = manager.Create();
    manager.MarkForDestroy(entity);
    manager.ProcessDeferredDestroy();

    BGE_TEST_FALSE(manager.IsAlive(entity));
    BGE_TEST_EQ(manager.AliveCount(), 0U);
    BGE_TEST_TRUE(manager.DeferredDestroyQueue().empty());
}

BGE_TEST(EntityManagerTests, DestroyIncrementsGeneration)
{
    EntityManager manager;

    const Entity entity = manager.Create();
    const u32 destroyedIndex = entity.index;
    const u32 destroyedGeneration = entity.generation;

    manager.MarkForDestroy(entity);
    manager.ProcessDeferredDestroy();

    const Entity recreated = manager.Create();

    BGE_TEST_EQ(recreated.index, destroyedIndex);
    BGE_TEST_EQ(recreated.generation, destroyedGeneration + 1U);
    BGE_TEST_TRUE(manager.IsAlive(recreated));
    BGE_TEST_FALSE(manager.IsAlive(entity));
}

BGE_TEST(EntityManagerTests, ReusedSlotUsesLowestAvailableIndex)
{
    EntityManager manager;

    const Entity a = manager.Create(); // 0
    const Entity b = manager.Create(); // 1
    const Entity c = manager.Create(); // 2
    const Entity d = manager.Create(); // 3

    manager.MarkForDestroy(c);
    manager.MarkForDestroy(a);
    manager.ProcessDeferredDestroy();

    const Entity first = manager.Create();
    const Entity second = manager.Create();

    BGE_TEST_EQ(first.index, 0U);
    BGE_TEST_EQ(second.index, 2U);

    BGE_TEST_TRUE(manager.IsAlive(first));
    BGE_TEST_TRUE(manager.IsAlive(second));
    BGE_TEST_TRUE(manager.IsAlive(b));
    BGE_TEST_TRUE(manager.IsAlive(d));
}

BGE_TEST(EntityManagerTests, DoubleMarkForDestroyDoesNotDuplicateQueueEntry)
{
    EntityManager manager;

    const Entity entity = manager.Create();

    manager.MarkForDestroy(entity);
    manager.MarkForDestroy(entity);

    BGE_TEST_EQ(manager.DeferredDestroyQueue().size(), static_cast<usize>(1));
}

BGE_TEST(EntityManagerTests, MarkForDestroyIgnoresInvalidEntity)
{
    EntityManager manager;

    manager.MarkForDestroy(Entity::Invalid());

    BGE_TEST_TRUE(manager.DeferredDestroyQueue().empty());
    BGE_TEST_EQ(manager.AliveCount(), 0U);
}

BGE_TEST(EntityManagerTests, MarkForDestroyIgnoresDeadEntity)
{
    EntityManager manager;

    const Entity entity = manager.Create();

    manager.MarkForDestroy(entity);
    manager.ProcessDeferredDestroy();
    manager.MarkForDestroy(entity);

    BGE_TEST_TRUE(manager.DeferredDestroyQueue().empty());
    BGE_TEST_EQ(manager.AliveCount(), 0U);
}

BGE_TEST(EntityManagerTests, ExistsChecksCapacityOnly)
{
    EntityManager manager;

    const Entity entity = manager.Create();

    BGE_TEST_TRUE(manager.Exists(entity.index));
    BGE_TEST_FALSE(manager.Exists(entity.index + 100U));
}

BGE_TEST(EntityManagerTests, AliveMaskReflectsCurrentState)
{
    EntityManager manager;

    const Entity a = manager.Create();
    const Entity b = manager.Create();

    BGE_TEST_TRUE(manager.AliveMask()[a.index]);
    BGE_TEST_TRUE(manager.AliveMask()[b.index]);

    manager.MarkForDestroy(a);
    manager.ProcessDeferredDestroy();

    BGE_TEST_FALSE(manager.AliveMask()[a.index]);
    BGE_TEST_TRUE(manager.AliveMask()[b.index]);
}

BGE_TEST(EntityManagerTests, OldHandleBecomesInvalidAfterReuse)
{
    EntityManager manager;

    const Entity oldEntity = manager.Create();

    manager.MarkForDestroy(oldEntity);
    manager.ProcessDeferredDestroy();

    const Entity newEntity = manager.Create();

    BGE_TEST_EQ(oldEntity.index, newEntity.index);
    BGE_TEST_TRUE(oldEntity.generation != newEntity.generation);

    BGE_TEST_FALSE(manager.IsAlive(oldEntity));
    BGE_TEST_TRUE(manager.IsAlive(newEntity));
}

BGE_TEST(EntityManagerTests, CapacityDoesNotShrinkOnDestroy)
{
    EntityManager manager;

    const Entity a = manager.Create();
    const Entity b = manager.Create();

    const u32 capacityBeforeDestroy = manager.Capacity();

    manager.MarkForDestroy(a);
    manager.MarkForDestroy(b);
    manager.ProcessDeferredDestroy();

    BGE_TEST_EQ(manager.Capacity(), capacityBeforeDestroy);
    BGE_TEST_EQ(manager.AliveCount(), 0U);
}