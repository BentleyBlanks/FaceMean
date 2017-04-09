#include "ofApp.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <algorithm>
#include <functional>
#include <vector>

//const char* drawWay = ;

int wayToDraw = 0;
int visualizeType = 0;
bool drawLine = true;
bool drawTriangle3 = false;
bool drawFace = false;
bool drawPoints = false;
bool drawPolyline = false;

std::vector<Triangle*> shapes;
std::vector<ofVec3f> points;
ofPolyline polyline;

std::vector<Triangle*> load(const aiScene* scene)
{
    if(!scene)
    {
        return std::vector<Triangle*>();
    }

    if(scene->mNumMeshes < 0)
    {
        //a3Log::error("模型中无任何图元对象\n");
        return std::vector<Triangle*>();
    }

    for(int m = 0; m < scene->mNumMeshes; m++)
    {
        const struct aiMesh* mesh = scene->mMeshes[m];

        //a3Log::debug("faces: %d, vertices: %d, normals: %d\n", mesh->mNumFaces, mesh->mNumVertices, mesh->mNumVertices * 3);
        // 仅支持三角形作为图元
        for(int i = 0; i < mesh->mNumFaces; i++)
        {
            //a3Log::info("Loading...%8.2f \r", (float) i / mesh->mNumFaces * 100.0f);

            int numIndices = mesh->mFaces[i].mNumIndices;
            if(numIndices != 3)
            {
                //a3Log::error("Atmos仅支持三角形作为片元, 而当前模型面由%d个顶点组成\n", numIndices);

                //A3_SAFE_DELETE(shapes);

                return std::vector<Triangle*>();
            }

            Triangle* triangle = new Triangle();

            int indexV0 = mesh->mFaces[i].mIndices[0], indexV1 = mesh->mFaces[i].mIndices[1], indexV2 = mesh->mFaces[i].mIndices[2];

            // init vertices
            triangle->setV1(mesh->mVertices[indexV0].x, mesh->mVertices[indexV0].y, mesh->mVertices[indexV0].z);
            triangle->setV2(mesh->mVertices[indexV1].x, mesh->mVertices[indexV1].y, mesh->mVertices[indexV1].z);
            triangle->setV3(mesh->mVertices[indexV2].x, mesh->mVertices[indexV2].y, mesh->mVertices[indexV2].z);

            //// init normals
            //triangle->n0.set(mesh->mNormals[indexV0].x, mesh->mNormals[indexV0].y, mesh->mNormals[indexV0].z);
            //triangle->n1.set(mesh->mNormals[indexV1].x, mesh->mNormals[indexV1].y, mesh->mNormals[indexV1].z);
            //triangle->n2.set(mesh->mNormals[indexV2].x, mesh->mNormals[indexV2].y, mesh->mNormals[indexV2].z);

            //// init texture coordinate
            //if(mesh->mTextureCoords[0])
            //{
            //    triangle->vt0.set(mesh->mTextureCoords[0][indexV0].x, mesh->mTextureCoords[0][indexV0].y, mesh->mTextureCoords[0][indexV0].z);
            //    triangle->vt1.set(mesh->mTextureCoords[0][indexV1].x, mesh->mTextureCoords[0][indexV1].y, mesh->mTextureCoords[0][indexV1].z);
            //    triangle->vt2.set(mesh->mTextureCoords[0][indexV2].x, mesh->mTextureCoords[0][indexV2].y, mesh->mTextureCoords[0][indexV2].z);
            //}

            triangle->setCenter();

            shapes.push_back(triangle);
        }

        //a3Log::print("\n");
        //a3Log::debug("Loading model successed\n");
    }

    //aiReleaseImport(scene);

    return shapes;
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(50, 50, 50, 0);

    //we need to call this for textures to work on models
    //ofDisableArbTex();

    //this makes sure that the back of the model doesn't show through the front
    ofEnableDepthTest();

    model.loadModel("CD005(1)/cd005.obj");
    model.setPosition(0, 0, 0);
    //printf("%d\n", model.getMeshCount());
    //ofMesh mesh = model.getMesh(0);
    shapes = load(model.getAssimpScene());

    light.setPosition(model.getPosition() + ofPoint(0, 0, 1600));

    graph = new Graph3D(400, 400, 30, 30);
    cam = new ofEasyCam();

    gui.setup();

    triangle1.set(ofVec3f(0, 0, 10), ofVec3f(50, 0, 0), ofVec3f(), ofVec3f(0, 50, 0));

    triangle2.set(ofVec3f(10, 0, 0), ofVec3f(0, 0, 50), ofVec3f(), ofVec3f(0, 50, 0));

    triangle3.set(ofVec3f(10, 0, 0), ofVec3f(200, 500, 0), ofVec3f(-500, 0, 0), ofVec3f(200, -500, 0));
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    switch(visualizeType)
    {
    case 0:
        drawWithModel();
        break;
    case 1:
        drawTriangle();
        break;
    case 2:
        drawTriangles();
    }

    gui.begin();

    //int scene = 0;
    if(ImGui::Combo("Scene Change", &visualizeType, "Model\0Triangle Test\0Triangles"))
    {
        //if(visualizeType == 0)
        //{
        //    ofDisableLighting();
        //    visualizeType = 1;
        //}
        //else if(visualizeType == 1)
        //{
        //    visualizeType = 0;
        //}
    }        

    ImGui::NewLine();
    if(ImGui::Button("Lock Camera"))
    {
        if(cam->getMouseInputEnabled())
            cam->disableMouseInput();
        else
            cam->enableMouseInput();
    }

    //ImGui::NewLine();
    //if(ImGui::Checkbox("Draw Line", &drawLine))
    //{
    //    if(drawLine)
    //        ofNoFill();
    //    else
    //        ofFill();
    //}

    switch(visualizeType)
    {
    case 0:
        ImGui::Combo("Way To Render", &wayToDraw, "Draw Faces\0Draw WireFrame\0Draw Vertices");

        break;
    case 1:
        ImGui::SliderFloat3("Triangle1 v1", triangle1.Vertex_1, -400, 400);
        ImGui::SliderFloat3("Triangle1 v2", triangle1.Vertex_2, -400, 400);
        ImGui::SliderFloat3("Triangle1 v3", triangle1.Vertex_3, -400, 400);

        ImGui::NewLine();

        ImGui::SliderFloat3("Triangle1 n", triangle1.Normal_0, -400, 400);

        ImGui::NewLine();

        ImGui::SliderFloat3("Triangle2 v1", triangle2.Vertex_1, -400, 400);
        ImGui::SliderFloat3("Triangle2 v2", triangle2.Vertex_2, -400, 400);
        ImGui::SliderFloat3("Triangle2 v3", triangle2.Vertex_3, -400, 400);

        ImGui::NewLine();

        ImGui::SliderFloat3("Triangle2 n", triangle2.Normal_0, -400, 400);

        break;
    case 2:   
        ImGui::Checkbox("Draw Triangle3", &drawTriangle3);
        ImGui::Checkbox("Draw Face", &drawFace);
        ImGui::Checkbox("Draw Points", &drawPoints);
        ImGui::Checkbox("Draw PolyLine", &drawPolyline);

        ImGui::SliderFloat3("Triangle3 v1", triangle3.Vertex_1, -400, 400);
        ImGui::SliderFloat3("Triangle3 v2", triangle3.Vertex_2, -400, 400);
        ImGui::SliderFloat3("Triangle3 v3", triangle3.Vertex_3, -400, 400);

        ImGui::NewLine();

        ImGui::SliderFloat3("Triangle3 n", triangle3.Normal_0, -400, 400);

        //if(ImGui::Button("Save"))
        //{
        //    //char* pathX0 = "E:\\Program\\Matlab\\x0.txt", *pathX1 = "E:\\Program\\Matlab\\x1.txt";
        //    //char* pathY0 = "E:\\Program\\Matlab\\y0.txt", *pathY1 = "E:\\Program\\Matlab\\y1.txt"; // 你要创建文件的路径
        //    //ofstream foutX0(pathX0), foutX1(pathX1), foutYH0(pathY0), foutYL0(pathY1);
        //    //if(foutX0)
        //    //{
        //    //    for(int i = 0; i < points.size(); i++)
        //    //    {
        //    //        if(points[i].y > 0)
        //    //            foutX0 << points[i].x << endl;
        //    //    }
        //    //    foutX0.close();
        //    //}
        //    //
        //    //if(foutX1)
        //    //{
        //    //    for(int i = 0; i < points.size(); i++)
        //    //    {
        //    //        if(points[i].y < 0)
        //    //            foutX1 << points[i].x << endl;
        //    //    }
        //    //    foutX1.close();
        //    //}
        //    //
        //    //if(foutYH0)
        //    //{
        //    //    for(int i = 0; i < points.size(); i++)
        //    //    {
        //    //        if(points[i].y > 0)
        //    //            foutYH0 << points[i].y << endl;
        //    //    }
        //    //    foutYH0.close();
        //    //}
        //    //
        //    //if(foutYL0)
        //    //{
        //    //    for(int i = 0; i < points.size(); i++)
        //    //    {
        //    //        if(points[i].y < 0)
        //    //            foutYL0 << points[i].y << endl;
        //    //    }
        //    //    foutYL0.close();
        //    //}


        //    char* pathX = "E:\\Program\\Matlab\\x.txt", 
        //        *pathY = "E:\\Program\\Matlab\\y.txt", 
        //        *pathZ = "E:\\Program\\Matlab\\z.txt";
        //
        //    ofstream foutX(pathX), foutY(pathY), foutZ(pathZ);
        //
        //    if(foutX)
        //    {
        //        for(int i = 0; i < shapes.size(); i++)
        //        {
        //            foutX << shapes[i]->center[0] << endl;
        //        }
        //        foutX.close();
        //    }
        //
        //    if(foutY)
        //    {
        //        for(int i = 0; i < shapes.size(); i++)
        //        {
        //            foutY << shapes[i]->center[1] << endl;
        //        }
        //        foutY.close();
        //    }
        //
        //    if(foutZ)
        //    {
        //        for(int i = 0; i < shapes.size(); i++)
        //        {
        //            foutZ << shapes[i]->center[2] << endl;
        //        }
        //        foutZ.close();
        //    }
        //}

        break;
    }

    gui.end();
}

//--------------------------------------------------------------
void ofApp::drawTriangle()
{
    cam->begin();
    ofPushStyle(); 

    ofEnableLighting();
    light.enable();

    //ofSetColor(255, 0, 0);
    ofSetColor(255, 255, 0);
    triangle1.drawNormal();

    //ofSetColor(0, 0, 255);
    ofSetColor(0, 255, 255);
    triangle2.drawNormal();

    //ofNoFill();

    TopologicalStructure t = judge_triangle_topologicalStructure(&triangle1, &triangle2);
    if(t == NONINTERSECT)
    {
        ofSetColor(255, 255, 255);
    }
    else
    {
        ofSetColor(255, 0, 0);
    }

    triangle1.draw();
    triangle2.draw();

    light.disable();
    ofDisableLighting();
    ofPopStyle();
    cam->end();
}

int checkQuadrant(const ofVec3f& v)
{
    if(v == ofVec3f::zero())
        return 0;

    if(v.x > 0 && v.y > 0)
    {
        return 1;
    }
    else if(v.x > 0 && v.y < 0)
    {
        return 2;
    }
    else if(v.x < 0 && v.y < 0)
    {
        return 3;
    }
    else
    {
        return 4;
    }
}

float angle(ofVec3f o, ofVec3f v)
{
    float dx = v.x - o.x;
    float dy = v.y - o.y;
    float r = sqrt(dx * dx + dy * dy);
    float angle = atan2(dy, dx);
    //angle -= base_angle;
    if(angle < 0) angle += PI * 2;
    return angle;
}

// m1 与x轴正方向夹角是否小于m2
bool sortPoints(const ofVec3f & m1, const ofVec3f & m2)
{
    //// 象限判定
    //int q1 = checkQuadrant(m1);
    //int q2 = checkQuadrant(m2);
    //
    //// --!
    //if(q1 == 0 || q2 == 0)
    //    return false;
    //
    //if(q1 > q2)
    //{
    //    return false;
    //}
    //else if(q2 > q1)
    //{
    //    return true;
    //}
    //else
    //{
    //    ofVec3f x;
    //    float theta1 = 0.0f, theta2 = 0.0f;
    //    // 同一象限 q1 == q2
    //    // 3 4象限
    //    if(q1 >= 3)
    //    {
    //        x.set(-1, 0, 0);
    //    }
    //    else
    //    {
    //        // 1 2象限
    //       x.set(1, 0, 0);
    //    }
    //
    //    theta1 = m1.getNormalized().dot(x);
    //    theta2 = m2.getNormalized().dot(x);
    //
    //    return theta1 > theta2;
    //}

    ofVec3f xDir(1, 0, 0);
    ofVec3f cross1 = xDir.crossed(m1.getNormalized());
    ofVec3f cross2 = xDir.crossed(m2.getNormalized());

   /* if(cross1.z == 0 && cross2.z != 0)
    {
        return true;
    }
    else if(cross1.z != 0 && cross2.z == 0)
    {
        return false;
    }
    else if(cross1.z == 0 && cross2.z == 0)
    {
        return false;
    }*/

    if(cross1.z > 0 && cross2.z < 0)
    {
        return true;
    }
    else if(cross2.z > 0 && cross1.z < 0)
    {
        return false;
    }
    else if(cross1.z > 0 && cross2.z > 0)
    {
        float cos1 = m1.getNormalized().dot(xDir);
        float cos2 = m2.getNormalized().dot(xDir);

        if(cos1 > cos2)
            return true;
        else
            return false;
    }
    else
    {
        float cos1 = m1.getNormalized().dot(xDir);
        float cos2 = m2.getNormalized().dot(xDir);

        if(cos1 > cos2)
            return false;
        else
            return true;
    }
}

//--------------------------------------------------------------
void ofApp::drawTriangles()
{
    cam->begin();
    ofPushStyle();

    ofDrawAxis(400);

    if(drawTriangle3)
    {
        ofFill();
        ofSetColor(0, 255, 0);
        triangle3.draw();
    }

    points.clear();
    ofNoFill();
    TopologicalStructure t;
    for(int i = 0; i < shapes.size(); i++)
    {
        t = judge_triangle_topologicalStructure(&triangle3, shapes[i]);
        if(t == NONINTERSECT)
        {
            ofSetColor(255, 255, 255);
        }
        else
        {
            ofVec3f pointProject(shapes[i]->center[0], shapes[i]->center[1], 0);
            ofVec3f point(shapes[i]->center[0], shapes[i]->center[1], shapes[i]->center[2]);

            points.push_back(pointProject);

            if(drawPoints)
            {
                ofFill();
                ofSetColor(0, 255, 0);
                ofDrawSphere(point, 0.15);

                ofSetColor(255, 0, 0);
                ofDrawSphere(pointProject, 0.05);

                //ofSetColor(255, 255, 255);
                //ofLine(ofVec3f(), ofVec3f(point.x, point.y, 0));

                ofNoFill();
            }

            ofSetColor(255, 0, 0);
        }

        if(drawFace)
        {
            shapes[i]->draw();
        }
    }

    if(drawPolyline)
    {
        if(!points.empty())
        {
            // 根据点与x轴角度排序
            sort(points.begin(), points.end(), sortPoints);

            ofSetColor(255, 255, 255);
            polyline.clear();
            polyline.addVertices(points);
            polyline.draw();
        }
    }

    //if(drawPoints)
    //{
    //    for(int i = 0; i < points.size(); i++)
    //    {
    //        ofDrawSphere(points[i], 1.2);
    //    }
    //}

    ofPopStyle();
    cam->end();
}

//--------------------------------------------------------------
void ofApp::drawWithModel()
{
    cam->begin();
    ofEnableLighting();

    light.enable();
    //drawWithMesh();
    //graph->draw();
    switch(wayToDraw)
    {
    case 0:
        model.drawFaces();
        break;
    case 1:
        model.drawWireframe();
        break;
    case 2:
        model.drawVertices();
        break;
    }
    //drawWithModel();

    light.disable();
    ofDisableLighting();
    cam->end();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
