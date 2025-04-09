#include "MPointer.h"
#include "LinkedList.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

// Función para probar operaciones básicas de MPointers
void testBasicOperations() {
    std::cout << "\n===== PRUEBA DE OPERACIONES BÁSICAS =====\n" << std::endl;
    
    // Crear e inicializar MPointers
    std::cout << "Creando MPointers..." << std::endl;
    MPointer<int> intPtr = MPointer<int>::New();
    MPointer<double> doublePtr = MPointer<double>::New();
    MPointer<char[64]> stringPtr = MPointer<char[64]>::New();
    
    // Asignar valores
    std::cout << "Asignando valores..." << std::endl;
    *intPtr = 42;
    *doublePtr = 3.14159;
    
    char testString[64] = "Hola, mundo!";
    bool setResult = GRPCClient::getInstance().Set(&stringPtr, testString, strlen(testString) + 1);
    std::cout << "Establecer string: " << (setResult ? "Éxito" : "Fallido") << std::endl;
    
    // Leer valores
    std::cout << "Leyendo valores..." << std::endl;
    int intValue = *intPtr;
    double doubleValue = *doublePtr;
    
    char retrievedString[64];
    size_t actualSize = 0;
    bool getResult = GRPCClient::getInstance().Get(&stringPtr, retrievedString, sizeof(retrievedString), actualSize);
    
    // Mostrar resultados
    std::cout << "Valor int: " << intValue << std::endl;
    std::cout << "Valor double: " << doubleValue << std::endl;
    std::cout << "Valor string: " << retrievedString << " (éxito: " << (getResult ? "Sí" : "No") << ")" << std::endl;
    
    // Prueba de asignación de puntero
    std::cout << "\nPrueba de asignación de puntero..." << std::endl;
    MPointer<int> intPtr2 = intPtr;  // Debe incrementar refCount
    std::cout << "Dirección intPtr: " << &intPtr << ", Dirección intPtr2: " << &intPtr2 << std::endl;
    std::cout << "Valor a través de intPtr2: " << *intPtr2 << std::endl;
    
    // Los destructores se llamarán automáticamente al salir de esta función
}

// Función para probar LinkedList usando MPointers
void testLinkedList() {
    std::cout << "\n===== PRUEBA DE LINKED LIST =====\n" << std::endl;
    
    // Crear una lista enlazada
    LinkedList<int> list;
    
    // Agregar elementos
    std::cout << "Agregando elementos a la lista..." << std::endl;
    for (int i = 0; i < 5; ++i) {
        list.add(i * 10);
        std::cout << "Agregado: " << (i * 10) << ", Tamaño de la lista: " << list.getSize() << std::endl;
    }
    
    // Mostrar elementos
    std::cout << "\nMostrando la lista:" << std::endl;
    list.display();
    
    // Obtener elementos por índice
    std::cout << "\nAcceso a elementos por índice:" << std::endl;
    for (int i = 0; i < list.getSize(); ++i) {
        std::cout << "list[" << i << "] = " << list.get(i) << std::endl;
    }
    
    // Eliminar elementos
    std::cout << "\nEliminando elementos..." << std::endl;
    list.remove(2); // Eliminar el elemento en el índice 2
    std::cout << "Después de eliminar el índice 2:" << std::endl;
    list.display();
    
    list.remove(0); // Eliminar el primer elemento
    std::cout << "Después de eliminar el primer elemento:" << std::endl;
    list.display();
    
    // La lista se destruirá automáticamente al salir de esta función
    // Esto debería decrementar los refCounts de todos los MPointers
}

// Función para probar el recolector de basura
void testGarbageCollection() {
    std::cout << "\n===== PRUEBA DEL RECOLECTOR DE BASURA =====\n" << std::endl;
    
    std::cout << "Creando MPointers temporales..." << std::endl;
    
    // Crear ámbito para que los punteros se destruyan automáticamente
    {
        MPointer<int> tempPtr1 = MPointer<int>::New();
        MPointer<int> tempPtr2 = MPointer<int>::New();
        
        *tempPtr1 = 100;
        *tempPtr2 = 200;
        
        std::cout << "tempPtr1 = " << *tempPtr1 << ", tempPtr2 = " << *tempPtr2 << std::endl;
        std::cout << "Saliendo del ámbito, los punteros serán liberados..." << std::endl;
        
        // Ambos punteros se destruirán aquí, decrementando su refCount a 0
    }
    
    std::cout << "Esperando al recolector de basura..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Los bloques de memoria deberían ser liberados por el recolector de basura
    std::cout << "Los bloques de memoria deberían haber sido liberados." << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <puerto>" << std::endl;
        return 1;
    }
    
    int port = std::atoi(argv[1]);
    std::cout << "Conectando al Memory Manager en el puerto " << port << "..." << std::endl;
    
    try {
        // Inicializar conexión al Memory Manager
        MPointer<int>::Init(port);
        std::cout << "Conexión establecida." << std::endl;
        
        // Ejecutar pruebas
        testBasicOperations();
        testLinkedList();
        testGarbageCollection();
        
        std::cout << "\nTodas las pruebas completadas." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}