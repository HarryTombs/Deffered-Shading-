#include <QMouseEvent>
#include <QGuiApplication>
#include <gtest/gtest.h>
#include <NGLScene.h>
#include <Mesh.h>
#include <FirstPersonCamera.h>

TEST(Mesh, Transform)
{
    Mesh mesh;
    mesh.Transform(1.0,1.0,1.0);
    EXPECT_EQ(mesh.m_TRANS.getPosition().m_x,1.0);
    EXPECT_EQ(mesh.m_TRANS.getPosition().m_y,1.0);
    EXPECT_EQ(mesh.m_TRANS.getPosition().m_z,1.0);
}
TEST(FPCamera, set)
{
    FirstPersonCamera Cam;
    Cam.set(ngl::Vec3(0.0,0.0,0.0),ngl::Vec3(0.0,0.0,-5.0),ngl::Vec3(0.0,1.0,0.0));
    EXPECT_EQ(Cam.camPos.m_x,0.0);
    EXPECT_EQ(Cam.camPos.m_y,0.0);
    EXPECT_EQ(Cam.camPos.m_z,0.0);
    EXPECT_EQ(Cam.camTo.m_x,0.0);
    EXPECT_EQ(Cam.camTo.m_y,0.0);
    EXPECT_EQ(Cam.camTo.m_z,-5.0);
    EXPECT_EQ(Cam.camUp.m_x,0.0);
    EXPECT_EQ(Cam.camUp.m_y,1.0);
    EXPECT_EQ(Cam.camUp.m_z,0.0);
}