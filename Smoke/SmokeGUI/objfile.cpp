#include "objfile.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>

OBJFile::OBJFile(QString fileName) {
    qDebug() << "✓✓ OBJFile constructor";

    qDebug() << ":: Loading" << fileName;
    QFile newModel(fileName);
    qDebug() << QDir::currentPath();
    if(newModel.open(QIODevice::ReadOnly)) {
        QTextStream fileContents(&newModel);

        QString currentLine;
        QStringList values;
        QStringList indices;

        unsigned short k;

        vertexCoords.clear();
        textureCoords.clear();
        vertexNormals.clear();
        faceCoordInd.clear();
        faceTexInd.clear();
        faceNormalInd.clear();
        while(!fileContents.atEnd()) {

            currentLine = fileContents.readLine();
            values = currentLine.split(" ");
            if (values[0] == "v") {
                // Only x, y and z. If there's a w value (homogenous coordinates), ignore it.
                vertexCoords.append(QVector3D(values[1].toFloat(), values[2].toFloat(), values[3].toFloat() ));
            }
            else if (values[0] == "vt") {
                // Only u and v. If there's a w value (barycentric coordinates), ignore it, it can be retrieved from 1-u-v.
                textureCoords.append(QVector2D(values[1].toFloat(), values[2].toFloat() ));
            }
            else if (values[0] == "vn") {
                vertexNormals.append(QVector3D(values[1].toFloat(), values[2].toFloat(), values[3].toFloat() ));
            }
            else if (values[0] == "f") {
                for (k=1; k<values.size(); k++) {
                    indices = values[k].split("/");

                    // Note -1, OBJ starts indexing from 1.

                    faceCoordInd.append(indices[0].toInt() -1 );

                    if (indices.size() > 1) {
                        if (!indices[1].isEmpty()) {
                            faceTexInd.append(indices[1].toInt() -1 );
                        }

                        if (indices.size() > 2) {
                            if (!indices[2].isEmpty()) {
                                faceNormalInd.append(indices[2].toInt() -1 );
                            }
                        }
                    }

                }

                faceValences.append(k-1);

            }
            else {
                qDebug() << " * Line contents ignored," << currentLine;
            }

        }

        newModel.close();

    }
    qDebug() << "loaded" << vertexCoords.length();
}

OBJFile::~OBJFile() {
    qDebug() << "✗✗ OBJFile destructor";
}
