#include "MainWindow.h"
#include <QMessageBox>
#include <iostream>
#include <string>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setWindowTitle("MPointers 2.0 Client");
    resize(800, 600);
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    // Main widget and layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Connection group
    QGroupBox *connectionGroup = new QGroupBox("Memory Manager Connection", centralWidget);
    QHBoxLayout *connectionLayout = new QHBoxLayout(connectionGroup);
    
    QLabel *portLabel = new QLabel("Port:", connectionGroup);
    portLineEdit = new QLineEdit("50051", connectionGroup);
    connectButton = new QPushButton("Connect", connectionGroup);
    connectionStatusLabel = new QLabel("Not Connected", connectionGroup);
    connectionStatusLabel->setStyleSheet("color: red;");
    
    connectionLayout->addWidget(portLabel);
    connectionLayout->addWidget(portLineEdit);
    connectionLayout->addWidget(connectButton);
    connectionLayout->addWidget(connectionStatusLabel);
    
    // Create pointers group
    QGroupBox *createGroup = new QGroupBox("Create MPointers", centralWidget);
    QHBoxLayout *createLayout = new QHBoxLayout(createGroup);
    
    createIntButton = new QPushButton("Create Int MPointer", createGroup);
    createDoubleButton = new QPushButton("Create Double MPointer", createGroup);
    createStringButton = new QPushButton("Create String MPointer", createGroup);
    
    createLayout->addWidget(createIntButton);
    createLayout->addWidget(createDoubleButton);
    createLayout->addWidget(createStringButton);
    
    // MPointer operations group
    QGroupBox *operationsGroup = new QGroupBox("MPointer Operations", centralWidget);
    QGridLayout *operationsLayout = new QGridLayout(operationsGroup);
    
    QLabel *pointerLabel = new QLabel("Select Pointer:", operationsGroup);
    pointerSelector = new QComboBox(operationsGroup);
    QLabel *valueLabel = new QLabel("Value:", operationsGroup);
    valueLineEdit = new QLineEdit(operationsGroup);
    setValueButton = new QPushButton("Set Value", operationsGroup);
    getValueButton = new QPushButton("Get Value", operationsGroup);
    QLabel *resultLabel = new QLabel("Current Value:", operationsGroup);
    valueDisplayLabel = new QLabel("", operationsGroup);
    valueDisplayLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    
    operationsLayout->addWidget(pointerLabel, 0, 0);
    operationsLayout->addWidget(pointerSelector, 0, 1);
    operationsLayout->addWidget(valueLabel, 1, 0);
    operationsLayout->addWidget(valueLineEdit, 1, 1);
    operationsLayout->addWidget(setValueButton, 2, 0);
    operationsLayout->addWidget(getValueButton, 2, 1);
    operationsLayout->addWidget(resultLabel, 3, 0);
    operationsLayout->addWidget(valueDisplayLabel, 3, 1);
    
    // Linked List group
    QGroupBox *listGroup = new QGroupBox("Linked List Operations", centralWidget);
    QGridLayout *listLayout = new QGridLayout(listGroup);
    
    QLabel *listValueLabel = new QLabel("Value to Add:", listGroup);
    listValueLineEdit = new QLineEdit(listGroup);
    listAddButton = new QPushButton("Add to List", listGroup);
    
    QLabel *listIndexLabel = new QLabel("Index:", listGroup);
    listIndexSpinBox = new QSpinBox(listGroup);
    listRemoveButton = new QPushButton("Remove from List", listGroup);
    
    listDisplayButton = new QPushButton("Display List", listGroup);
    listDisplayArea = new QTextEdit(listGroup);
    listDisplayArea->setReadOnly(true);
    
    listLayout->addWidget(listValueLabel, 0, 0);
    listLayout->addWidget(listValueLineEdit, 0, 1);
    listLayout->addWidget(listAddButton, 0, 2);
    listLayout->addWidget(listIndexLabel, 1, 0);
    listLayout->addWidget(listIndexSpinBox, 1, 1);
    listLayout->addWidget(listRemoveButton, 1, 2);
    listLayout->addWidget(listDisplayButton, 2, 0, 1, 3);
    listLayout->addWidget(listDisplayArea, 3, 0, 1, 3);
    
    // Add all groups to main layout
    mainLayout->addWidget(connectionGroup);
    mainLayout->addWidget(createGroup);
    mainLayout->addWidget(operationsGroup);
    mainLayout->addWidget(listGroup);
    
    // Set central widget
    setCentralWidget(centralWidget);
    
    // Connect signals and slots
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(createIntButton, &QPushButton::clicked, this, &MainWindow::onCreateIntPointerClicked);
    connect(createDoubleButton, &QPushButton::clicked, this, &MainWindow::onCreateDoublePointerClicked);
    connect(createStringButton, &QPushButton::clicked, this, &MainWindow::onCreateStringPointerClicked);
    connect(setValueButton, &QPushButton::clicked, this, &MainWindow::onSetValueClicked);
    connect(getValueButton, &QPushButton::clicked, this, &MainWindow::onGetValueClicked);
    connect(listAddButton, &QPushButton::clicked, this, &MainWindow::onLinkedListAddClicked);
    connect(listRemoveButton, &QPushButton::clicked, this, &MainWindow::onLinkedListRemoveClicked);
    connect(listDisplayButton, &QPushButton::clicked, this, &MainWindow::onLinkedListDisplayClicked);
    
    // Disable UI elements until connected
    createIntButton->setEnabled(false);
    createDoubleButton->setEnabled(false);
    createStringButton->setEnabled(false);
    setValueButton->setEnabled(false);
    getValueButton->setEnabled(false);
    listAddButton->setEnabled(false);
    listRemoveButton->setEnabled(false);
    listDisplayButton->setEnabled(false);
}

void MainWindow::onConnectClicked() {
    int port = portLineEdit->text().toInt();
    
    try {
        MPointer<int>::Init(port);
        connectionStatusLabel->setText("Connected");
        connectionStatusLabel->setStyleSheet("color: green;");
        
        // Enable UI elements
        createIntButton->setEnabled(true);
        createDoubleButton->setEnabled(true);
        createStringButton->setEnabled(true);
        setValueButton->setEnabled(true);
        getValueButton->setEnabled(true);
        listAddButton->setEnabled(true);
        listRemoveButton->setEnabled(true);
        listDisplayButton->setEnabled(true);
        
        // Disable connect button
        connectButton->setEnabled(false);
        
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Connection Error", 
                            QString("Failed to connect: %1").arg(e.what()));
    }
}

void MainWindow::onCreateIntPointerClicked() {
    try {
        MPointer<int> ptr = MPointer<int>::New();
        intPointers.push_back(ptr);
        updatePointerSelector();
        QMessageBox::information(this, "Success", "Created new int pointer");
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
                            QString("Failed to create pointer: %1").arg(e.what()));
    }
}

void MainWindow::onCreateDoublePointerClicked() {
    try {
        MPointer<double> ptr = MPointer<double>::New();
        doublePointers.push_back(ptr);
        updatePointerSelector();
        QMessageBox::information(this, "Success", "Created new double pointer");
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
                            QString("Failed to create pointer: %1").arg(e.what()));
    }
}

void MainWindow::onCreateStringPointerClicked() {
    try {
        MPointer<char[64]> ptr = MPointer<char[64]>::New();
        stringPointers.push_back(ptr);
        updatePointerSelector();
        QMessageBox::information(this, "Success", "Created new string pointer");
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
                            QString("Failed to create pointer: %1").arg(e.what()));
    }
}

void MainWindow::updatePointerSelector() {
    pointerSelector->clear();
    
    for (size_t i = 0; i < intPointers.size(); ++i) {
        pointerSelector->addItem(QString("Int_%1").arg(i));
    }
    
    for (size_t i = 0; i < doublePointers.size(); ++i) {
        pointerSelector->addItem(QString("Double_%1").arg(i));
    }
    
    for (size_t i = 0; i < stringPointers.size(); ++i) {
        pointerSelector->addItem(QString("String_%1").arg(i));
    }
}

void MainWindow::onSetValueClicked() {
    QString selected = pointerSelector->currentText();
    QString value = valueLineEdit->text();
    
    try {
        if (selected.startsWith("Int_")) {
            int index = selected.mid(4).toInt();
            int intValue = value.toInt();
            intPointers[index] = intValue;
            QMessageBox::information(this, "Success", "Value set successfully");
        }
        else if (selected.startsWith("Double_")) {
            int index = selected.mid(7).toInt();
            double doubleValue = value.toDouble();
            doublePointers[index] = doubleValue;
            QMessageBox::information(this, "Success", "Value set successfully");
        }
        else if (selected.startsWith("String_")) {
            int index = selected.mid(7).toInt();
            QByteArray ba = value.toUtf8();
            char stringValue[64] = {0};
            strncpy(stringValue, ba.constData(), std::min(63, ba.size()));
            
            char* strPtr = reinterpret_cast<char*>(&(stringPointers[index]));
            strncpy(strPtr, stringValue, 64);
            
            QMessageBox::information(this, "Success", "Value set successfully");
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
                            QString("Failed to set value: %1").arg(e.what()));
    }
}

void MainWindow::onGetValueClicked() {
    QString selected = pointerSelector->currentText();
    
    try {
        if (selected.startsWith("Int_")) {
            int index = selected.mid(4).toInt();
            int value = *intPointers[index];
            valueDisplayLabel->setText(QString::number(value));
        }
        else if (selected.startsWith("Double_")) {
            int index = selected.mid(7).toInt();
            double value = *doublePointers[index];
            valueDisplayLabel->setText(QString::number(value));
        }
        else if (selected.startsWith("String_")) {
            int index = selected.mid(7).toInt();
            char value[64];
            memcpy(value, &(*stringPointers[index]), 64);
            valueDisplayLabel->setText(QString(value));
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
                            QString("Failed to get value: %1").arg(e.what()));
    }
}

void MainWindow::onLinkedListAddClicked() {
    QString valueStr = listValueLineEdit->text();
    int value = valueStr.toInt();
    
    try {
        intList.add(value);
        listIndexSpinBox->setMaximum(intList.getSize() - 1);
        QMessageBox::information(this, "Success", "Value added to list");
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
                            QString("Failed to add to list: %1").arg(e.what()));
    }
}

void MainWindow::onLinkedListRemoveClicked() {
    int index = listIndexSpinBox->value();
    
    try {
        bool success = intList.remove(index);
        if (success) {
            listIndexSpinBox->setMaximum(std::max(0, intList.getSize() - 1));
            QMessageBox::information(this, "Success", "Value removed from list");
        } else {
            QMessageBox::warning(this, "Warning", "Failed to remove value (invalid index)");
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
                            QString("Failed to remove from list: %1").arg(e.what()));
    }
}

void MainWindow::onLinkedListDisplayClicked() {
    try {
        std::stringstream ss;
        ss << "List size: " << intList.getSize() << std::endl;
        ss << "Elements: ";
        
        for (int i = 0; i < intList.getSize(); ++i) {
            ss << intList.get(i);
            if (i < intList.getSize() - 1) {
                ss << " -> ";
            }
        }
        
        listDisplayArea->setText(QString::fromStdString(ss.str()));
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
                            QString("Failed to display list: %1").arg(e.what()));
    }
}