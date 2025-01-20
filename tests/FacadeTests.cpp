#include <catch2/catch_test_macros.hpp>
#include <string>
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
  std::string date = "20-10-2025";
  facade->newTask(description, Priority::Level::High, date);

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
  std::string date = "20-10-2025";
  facade->newTask("Toggle Task", Priority::Level::Low, date);
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

  std::string date = "20-10-2025";
  facade->newTask("Task to Remove", Priority::Level::Medium, date);
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

  std::string date = "20-10-2025";
  facade->newTask("Task to update", Priority::Level::Medium, date);
  auto tasks = facade->getTasks();

  REQUIRE(tasks[0]->getProgress() == 0);
  facade->setTaskProgress(tasks[0], 80);
  REQUIRE(tasks[0]->getProgress() == 80);
  facade->setTaskProgress(tasks[0], 100);
  REQUIRE(tasks[0]->isCompleted());
  facade->setTaskProgress(tasks[0], 50);
  REQUIRE(!tasks[0]->isCompleted());
}

TEST_CASE("Task due date", "[Facade]") {
  Facade::resetInstance();
  Facade *facade = Facade::getInstance();

  facade->overwriteTasker("/tmp/test_tasks.json");
  facade->cleanTaskerJsonFile();

  std::string date = "20-10-2025";
  facade->newTask("Task to update", Priority::Level::Medium, date);
  auto tasks = facade->getTasks();

  REQUIRE(tasks[0]->getFullDueDate() == "20-10-2025");
  auto chrono_time = facade->getTimeFromString("20-10-2025");
  auto task_time = facade->getTimeFromString(tasks[0]->getFullDueDate());
  REQUIRE(chrono_time == task_time);
}

TEST_CASE("Tags", "[Facade]") {
  Facade::resetInstance();
  Facade *facade = Facade::getInstance();

  facade->overwriteTasker("/tmp/test_tasks.json");
  facade->cleanTaskerJsonFile();

  std::string date = "20-10-2025";
  facade->newTask("Task with tag", Priority::Level::High, date);
  facade->newTask("Task with another tag", Priority::Level::Low, date, "work");
  auto tasks = facade->getTasks();

  REQUIRE(facade->getAllTags().size() == 2);
  REQUIRE(facade->getAllTags()[1] == "default");
  REQUIRE(facade->getTasksByTag("work").size() == 1);
  REQUIRE(facade->getTasksByTag("work")[0] == tasks[1]);
}
