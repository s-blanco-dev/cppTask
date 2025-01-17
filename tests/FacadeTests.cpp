#include <catch2/catch_test_macros.hpp>
#define CATCH_CONFIG_MAIN // Genera el main() para ejecutar las pruebas
#include "../include/Facade.h"
#include "../include/Priority.h"
#include "../include/Task.h"
#include "../include/TaskManager.h"
#include <catch2/catch_all.hpp>

// Helper para reiniciar el singleton antes de cada prueba
TEST_CASE("Singleton instance creation", "[Facade]") {
  Facade *instance1 = Facade::getInstance();
  Facade *instance2 = Facade::getInstance();

  REQUIRE(instance1 != nullptr);
  REQUIRE(instance1 == instance2); // Ambos deben ser la misma instancia
}

TEST_CASE("Task creation and retrieval using Facade", "[Facade]") {
  Facade::resetInstance();
  Facade *facade = Facade::getInstance();

  // Sobreescribimos el TaskManager para pruebas con un archivo vacío
  facade->overwriteTasker("/tmp/test_tasks.json");
  facade->cleanTaskerJsonFile();

  // Crear una nueva tarea
  std::string description = "Test Task";
  facade->newTask(description, Priority::Level::High);

  // Recuperar tareas y verificar
  auto tasks = facade->getTasks();
  REQUIRE(tasks.size() == 1);
  REQUIRE(tasks[0]->getDescription() == description);
  REQUIRE(tasks[0]->getPriority() == Priority::Level::High);
}

TEST_CASE("Task toggle completion using Facade", "[Facade]") {
  Facade::resetInstance();
  Facade *facade = Facade::getInstance();

  // Sobreescribimos el TaskManager para pruebas con un archivo vacío
  facade->overwriteTasker("/tmp/test_tasks.json");
  facade->cleanTaskerJsonFile();

  // Crear una tarea y alternar su estado
  facade->newTask("Toggle Task", Priority::Level::Low);
  auto tasks = facade->getTasks();

  REQUIRE(!tasks[0]->isCompleted());
  facade->toggleTaskCompleted(tasks[0]);
  REQUIRE(tasks[0]->isCompleted());
}

TEST_CASE("Task removal using Facade", "[Facade]") {
  Facade::resetInstance();
  Facade *facade = Facade::getInstance();

  facade->overwriteTasker("/tmp/test_tasks.json");
  facade->cleanTaskerJsonFile();

  facade->newTask("Task to Remove", Priority::Level::Medium);
  auto tasks = facade->getTasks();

  REQUIRE(tasks.size() == 1);

  facade->removeTask(tasks[0]);
  tasks = facade->getTasks();

  REQUIRE(tasks.empty());
}

TEST_CASE("Updating task progress", "[Facade]") {
  Facade::resetInstance();
  Facade *facade = Facade::getInstance();

  facade->overwriteTasker("/tmp/test_tasks.json");
  facade->cleanTaskerJsonFile();

  facade->newTask("Task to update", Priority::Level::Medium);
  auto tasks = facade->getTasks();

  REQUIRE(tasks[0]->getProgress() == 0);
  facade->setTaskProgress(tasks[0], 80);
  REQUIRE(tasks[0]->getProgress() == 80);
  facade->setTaskProgress(tasks[0], 100);
  REQUIRE(tasks[0]->isCompleted());
  facade->setTaskProgress(tasks[0], 50);
  REQUIRE(!tasks[0]->isCompleted());
}
