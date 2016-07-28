/*
Copyright(c) 2016 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= INCLUDES =================
#include "DirectusRigidBody.h"
//============================

//= NAMESPACES ================
using namespace Directus::Math;
//=============================

DirectusRigidBody::DirectusRigidBody(QWidget* parent): QWidget(parent)
{
    m_directusCore = nullptr;
    m_inspector = nullptr;
}

void DirectusRigidBody::Initialize(DirectusCore* directusCore, DirectusInspector* inspector)
{
    m_directusCore = directusCore;
    m_inspector = inspector;

    m_gridLayout = new QGridLayout();
    m_gridLayout->setMargin(4);

    //= TITLE =================================================
    m_title = new QLabel("RigidBody");
    m_title->setStyleSheet(
                "background-image: url(:/Images/rigidBody.png);"
                "background-repeat: no-repeat;"
                "background-position: left;"
                "padding-left: 20px;"
                );
    //=========================================================

    //= MASS =============================
    m_mass = new DirectusComboLabelText();
    m_mass->Initialize("Mass");
    //====================================

    //= DRAG =============================
    m_drag = new DirectusComboLabelText();
    m_drag->Initialize("Drag");
    //====================================

    //= ANGULAR DRAG =====================
    m_angularDrag = new DirectusComboLabelText();
    m_angularDrag->Initialize("Angular Drag");
    //====================================

    //= USE GRAVITY  =====================
    m_useGravityLabel = new QLabel("Use Gravity");
    m_useGravity = new QCheckBox();
    //====================================

    //= IS KINEMATIC  ====================
    m_isKinematicLabel = new QLabel("Is Kinematic");
    m_isKinematic = new QCheckBox();
    //====================================

    //= FREEZE POSITION ==================
    m_freezePosLabel = new QLabel("Freeze Position");
    m_freezePosXLabel= new QLabel("X");
    m_freezePosYLabel= new QLabel("Y");
    m_freezePosZLabel= new QLabel("Z");
    m_freezePosX = new QCheckBox();
    m_freezePosY = new QCheckBox();
    m_freezePosZ = new QCheckBox();
    //====================================

    //= FREEZE ROTATION ==================
    m_freezeRotLabel = new QLabel("Freeze Rotation");
    m_freezeRotXLabel= new QLabel("X");
    m_freezeRotYLabel= new QLabel("Y");
    m_freezeRotZLabel= new QLabel("Z");
    m_freezeRotX = new QCheckBox();
    m_freezeRotY = new QCheckBox();
    m_freezeRotZ = new QCheckBox();
    //====================================

    //= LINE ======================================
    m_line = new QWidget();
    m_line->setFixedHeight(1);
    m_line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_line->setStyleSheet(QString("background-color: #585858;"));
    //=============================================

    // addWidget(widget, row, column, rowspan, colspan)
    //= GRID ==================================================
    int row = 0;

    // Row 0 - TITLE
    m_gridLayout->addWidget(m_title, row, 0, 1, 1);
    row++;

    // Row 1 - MASS
    m_gridLayout->addWidget(m_mass->GetLabelWidget(), row, 0, 1, 1);
    m_gridLayout->addWidget(m_mass->GetTextWidget(), row, 1, 1, 1);
    row++;

    // Row 2 - DRAG
    m_gridLayout->addWidget(m_drag->GetLabelWidget(), row, 0, 1, 1);
    m_gridLayout->addWidget(m_drag->GetTextWidget(), row, 1, 1, 1);
    row++;

    // Row 3 - ANGULAR DRAG
    m_gridLayout->addWidget(m_angularDrag->GetLabelWidget(), row, 0, 1, 1);
    m_gridLayout->addWidget(m_angularDrag->GetTextWidget(), row, 1, 1, 1);
    row++;

    // Row 4 - USE GRAVITY
    m_gridLayout->addWidget(m_useGravityLabel, row, 0, 1, 1);
    m_gridLayout->addWidget(m_useGravity, row, 1, 1, 1);
    row++;

    // Row 5 - IS KINEMATIC
    m_gridLayout->addWidget(m_isKinematicLabel, row, 0, 1, 1);
    m_gridLayout->addWidget(m_isKinematic, row, 1, 1, 1);
    row++;

    // Row 6 - FREEZE POSITION
    m_gridLayout->addWidget(m_freezePosLabel,   row, 0, 1, 1);
    m_gridLayout->addWidget(m_freezePosXLabel,  row, 1, 1, 1);
    m_gridLayout->addWidget(m_freezePosX,       row, 2, 1, 1);
    m_gridLayout->addWidget(m_freezePosYLabel,  row, 3, 1, 1);
    m_gridLayout->addWidget(m_freezePosY,       row, 4, 1, 1);
    m_gridLayout->addWidget(m_freezePosZLabel,  row, 5, 1, 1);
    m_gridLayout->addWidget(m_freezePosZ,       row, 6, 1, 1);
    row++;

    // Row 7 - FREEZE ROTATION
    m_gridLayout->addWidget(m_freezeRotLabel,   row, 0, 1, 1);
    m_gridLayout->addWidget(m_freezeRotXLabel,  row, 1, 1, 1);
    m_gridLayout->addWidget(m_freezeRotX,       row, 2, 1, 1);
    m_gridLayout->addWidget(m_freezeRotYLabel,  row, 3, 1, 1);
    m_gridLayout->addWidget(m_freezeRotY,       row, 4, 1, 1);
    m_gridLayout->addWidget(m_freezeRotZLabel,  row, 5, 1, 1);
    m_gridLayout->addWidget(m_freezeRotZ,       row, 6, 1, 1);
    row++;

    // Row 8 - LINE
    m_gridLayout->addWidget(m_line, row, 0, 1, 7);
    //==============================================================================

    connect(m_mass,         SIGNAL(ValueChanged()),     this, SLOT(MapMass()));
    connect(m_drag,         SIGNAL(ValueChanged()),     this, SLOT(MapDrag()));
    connect(m_angularDrag,  SIGNAL(ValueChanged()),     this, SLOT(MapAngularDrag()));
    connect(m_useGravity,   SIGNAL(clicked(bool)),      this, SLOT(MapUseGravity()));
    connect(m_isKinematic,  SIGNAL(clicked(bool)),      this, SLOT(MapIsKinematic()));
    connect(m_freezePosX,   SIGNAL(clicked(bool)),      this, SLOT(MapFreezePosition()));
    connect(m_freezePosY,   SIGNAL(clicked(bool)),      this, SLOT(MapFreezePosition()));
    connect(m_freezePosZ,   SIGNAL(clicked(bool)),      this, SLOT(MapFreezePosition()));
    connect(m_freezeRotX,   SIGNAL(clicked(bool)),      this, SLOT(MapFreezeRotation()));
    connect(m_freezeRotY,   SIGNAL(clicked(bool)),      this, SLOT(MapFreezeRotation()));
    connect(m_freezeRotZ,   SIGNAL(clicked(bool)),      this, SLOT(MapFreezeRotation()));

    this->setLayout(m_gridLayout);
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    this->hide();
}

void DirectusRigidBody::Reflect(GameObject* gameobject)
{
    m_inspectedRigidBody = nullptr;

    // Catch evil case
    if (!gameobject)
    {
        this->hide();
        return;
    }

    m_inspectedRigidBody = gameobject->GetComponent<RigidBody>();
    if (!m_inspectedRigidBody)
    {
        this->hide();
        return;
    }

    // Do the actual reflection
    ReflectMass();
    ReflectDrag();
    ReflectAngulaDrag();
    ReflectUseGravity();
    ReflectIsKinematic();
    ReflectFreezePosition();
    ReflectFreezeRotation();

    // Make this widget visible
    this->show();
}

void DirectusRigidBody::ReflectMass()
{
    if (!m_inspectedRigidBody)
        return;

    float mass = m_inspectedRigidBody->GetMass();
    m_mass->SetFromFloat(mass);
}

void DirectusRigidBody::ReflectDrag()
{
    if (!m_inspectedRigidBody)
        return;

    float drag = m_inspectedRigidBody->GetDrag();
    m_drag->SetFromFloat(drag);
}

void DirectusRigidBody::ReflectAngulaDrag()
{
    if (!m_inspectedRigidBody)
        return;

    float angularDrag = m_inspectedRigidBody->GetAngularDrag();
    m_angularDrag->SetFromFloat(angularDrag);
}

void DirectusRigidBody::ReflectUseGravity()
{
    if (!m_inspectedRigidBody)
        return;

    bool gravity = m_inspectedRigidBody->GetUseGravity();
    m_useGravity->setChecked(gravity);
}

void DirectusRigidBody::ReflectIsKinematic()
{
    if (!m_inspectedRigidBody)
        return;

    bool kinematic = m_inspectedRigidBody->GetKinematic();
    m_isKinematic->setChecked(kinematic);
}

void DirectusRigidBody::ReflectFreezePosition()
{
    if (!m_inspectedRigidBody)
        return;

    Vector3 posFreeze = m_inspectedRigidBody->GetPositionLock();
    m_freezePosX->setChecked(bool(posFreeze.x));
    m_freezePosY->setChecked(bool(posFreeze.y));
    m_freezePosZ->setChecked(bool(posFreeze.z));
}

void DirectusRigidBody::ReflectFreezeRotation()
{
    if (!m_inspectedRigidBody)
        return;

    Vector3 rotFreeze = m_inspectedRigidBody->GetRotationLock();
    m_freezeRotX->setChecked(bool(rotFreeze.x));
    m_freezeRotY->setChecked(bool(rotFreeze.y));
    m_freezeRotZ->setChecked(bool(rotFreeze.z));
}

void DirectusRigidBody::MapMass()
{
    if (!m_inspectedRigidBody || !m_directusCore)
        return;

    float mass = m_mass->GetAsFloat();
    m_inspectedRigidBody->SetMass(mass);

    m_directusCore->Update();
}

void DirectusRigidBody::MapDrag()
{
    if (!m_inspectedRigidBody || !m_directusCore)
        return;

    float drag = m_drag->GetAsFloat();
    m_inspectedRigidBody->SetDrag(drag);

    m_directusCore->Update();
}

void DirectusRigidBody::MapAngularDrag()
{
    if (!m_inspectedRigidBody || !m_directusCore)
        return;

    float angularDrag = m_angularDrag->GetAsFloat();
    m_inspectedRigidBody->SetAngularDrag(angularDrag);

    m_directusCore->Update();
}

void DirectusRigidBody::MapUseGravity()
{
    if (!m_inspectedRigidBody || !m_directusCore)
        return;

    bool useGravity = m_useGravity->isChecked();
    m_inspectedRigidBody->SetUseGravity(useGravity);

    m_directusCore->Update();
}

void DirectusRigidBody::MapIsKinematic()
{
    if (!m_inspectedRigidBody || !m_directusCore)
        return;

    bool isKinematic = m_isKinematic->isChecked();
    m_inspectedRigidBody->SetKinematic(isKinematic);

    m_directusCore->Update();
}

void DirectusRigidBody::MapFreezePosition()
{
    if (!m_inspectedRigidBody || !m_directusCore)
        return;

    bool lockX = m_freezePosX->isChecked();
    bool lockY = m_freezePosY->isChecked();
    bool lockZ = m_freezePosZ->isChecked();
    m_inspectedRigidBody->SetPositionLock(Vector3(lockX, lockY, lockZ));

    m_directusCore->Update();
}

void DirectusRigidBody::MapFreezeRotation()
{
    if (!m_inspectedRigidBody || !m_directusCore)
        return;

    bool lockX = m_freezeRotX->isChecked();
    bool lockY = m_freezeRotY->isChecked();
    bool lockZ = m_freezeRotZ->isChecked();
    m_inspectedRigidBody->SetRotationLock(Vector3(lockX, lockY, lockZ));

    m_directusCore->Update();
}