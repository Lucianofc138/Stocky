#ifndef SHELF_UTILS_
#define SHELF_UTILS_

#include <iostream>
#include <ctime>  // timestamp

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/mat.hpp> 
#include <opencv2/core/types.hpp>
#include <opencv2/core/persistence.hpp> // JSON

#include "histUtils/histUtils.hpp"
#include "processingMisc/processingMisc.hpp"

class CrateModel
{
    public:
        /** 
         * Constructor de CrateModel
         */
        CrateModel();

        // CrateModel(cv::Mat srcImg_, cv::Rect reg_, cv::Mat templateImg_);
        /** 
         * Carga una imagen de template y una imagen actual del crate
         * @param srcImg_ imagen a cargar en el crate
         * @param reg_ rectangulo para seleccionar parte de la srcImg_
         * @param templateImg_ imagen del template a usar
         */
        void loadCrate(cv::Mat srcImg_, cv::Rect reg_, cv::Mat templateImg_);

        /** 
         * Actualiza el crate con una imagen dada
         * @param img_  imagen con la que actualizar
         */
        void updateCrateImg(cv::Mat img_);
    
        /** 
         * No se alcanzo a implementar en esta parte
         */
        void checkIfImageMatch();

        /** 
         * Setea crateImage_
         * @param crateImage_ parametro con el cual se setea
         */
        void setCrateImage(cv::Mat crateImage_);

        /** 
         * Setea TemplateImage
         * @param templImage_ parametro con el cual se setea
         */
        void setTemplateImage(cv::Mat templImage_);

        /** 
         * @return Matriz de imagen del Crate
         */
        cv::Mat getCrateImage();

        /** 
         * @return Matriz de imagen del TemplateImage correspondiente
         */
        cv::Mat getTemplateImage();

    private:
        void calcHist();
        cv::Mat templateImage;
        cv::Mat crateImage;
        // Hist histogram;
        std::time_t init_time;
        std::time_t finish_time;
};

class Crate
{
    public:
        /** 
         * Constructor de Crate
         * @param srcImg_ imagen a usar
         * @param reg_ rectangulo objetivo de la region
         * @param prodTmplt Template del producto
         */
        Crate(cv::Mat srcImg_, cv::Rect reg_, cv::Mat prodTmplt);

        /** 
         * Actualiza el crate con la imagen dada, chequea
         * modelos con funcion checkModels().
         * @param floorImg_ Imagen del piso de la repisa
         */
        void updateCrate(cv::Mat floorImg_);

        /** 
         * Actualiza la imagen del crate, la imagen entrega, hace uso de
         * regionInFloor, actualiza currentModel con la crateImg
         * @param floorImg_ Imagen del piso de la repisa
         */
        void updateCrateImg(cv::Mat floorImg_);

        /** 
         * Revisa si es que el producto es el correcto o incorrecto,
         * si es correcto isEmpty se hace falso
         */
        void checkModels();

        /** 
         * Calcula si es que el el sector actual esta vacio o no
         */
        void calcIfEmpty();

        /** 
         * @return rectangulo asociado a regionInFloor
         */
        cv::Rect getRect();
        
        /** 
         * @return matriz con la foto actual del sector
         */
        cv::Mat getCurrentImage();

        /** 
         * @return bool de isEmpty 
         */
        bool isItEmpty();

        /** 
         * @return bool de productIsRight
         */
        bool isItRight();


    private:
        cv::Rect regionInFloor;
        CrateModel currentModel;
        CrateModel rightModel;
        bool productIsRight;
        bool isEmpty;
        std::time_t lastUpdate;
        bool checkedSinceLastUpdate;
        
        void updateStatus();

        void checkCurrentModelTime();

        void updateRightModel();
        

};

class Floor
{ 
    public:
        /*
         * Constructor de Floor
         * @param shelfImage imagen de la repisa
         * @param rect rectangulo de los pisos de la repisa
         * @param fgMask_ mascara de foreground de la repisa
         * @param path_ ruta a archivo .json
         * @param productsNodes_ vector de nodos en el archivo .json
         * @param productsList_ vector de string con los nombres de los productos
         */
        Floor(cv::Mat shelfImage, cv::Rect rect, cv::Mat fgMask_,std::string path_, 
             std::vector< cv::FileNode > productsNodes_,
             std::vector< std::string > productsList_);

        /*
         * Llama a updateImage y setFgMask para actualizar la imagen del piso
         * de la repisa y su correspondiente mascara de foreground
         * @param shelfImage Imagen de la repisa
         * @param shelfFgMask_ mascara del foreground de la repisa
         */
        void updateFloor(cv::Mat shelfImage, cv::Mat shelfFgMask_);

        /*
         * Actualiza la imagen del piso con la imagen de la repisa 
         * y su rect correspondiente
         * @param shelfImage Imagen de la repisa
         */
        void updateImage(cv::Mat shelfImage);

        /*
         * Setea la mascara de foreground para cada piso en la repisa
         * @param shelfFgMask_ mascara del foreground de la repisa
         */
        void setFgMask(cv::Mat shelfFgMask_);

        /*
         * BORRAAAAAAAR
         */
        void setProducts( std::vector< std::string > products_ ); //BORRAAAAAAAR
        
        /*
         * Revisa si la mascara del Foreground esta 
         * interactuando con los pisos de la repisa
         */
        void checkIfFgIntersectsCrates();

        /*
         * Una ves que no hay movimiento actualiza los crates
         */
        void reactToMovementEnd();

        /*
         * Calcula la mascara de los espacios vacios en los pisos
         * @param src_img imagen de la repisa
         * @param emptyThreshold umbral para calcular vacios
         */
        void calcEmptyMask(cv::Mat src_img, int emptyThreshold);

        /*
         * Calcula si los distintos crates tienen un 
         * producto del template asociado, sacado del .json
         */
        void calcCrates();

        /*
         * Calcula la mascara de los distintos crates
         */
        void calcCrateMask();

        /*
         * Calcula la mascara de los productos incorrectos en la repisa
         */
        void calcWrongProductMask();

        cv::Rect getFloorRect();

        /*
         * @return Matriz con la imagen del piso actual
         */
        cv::Mat getFloorImage();

        /*
         * @return matriz de imagen de la mascara vacia
         */
        cv::Mat getEmptyMask();

        /*
         * @return matriz de la mascara de Foreground
         */
        cv::Mat getFgMask();

        /*
         * @return matriz de la mascara con los productos incorrectos
         */
        cv::Mat getWrongProductsMask();

        /*
         * @return bool que define si hay algo moviendose (true) o no (false)
         */
        bool isMoving();

        /*
         * @param index numero de crate a retornar
         * @return Crate elegido por index
         */
        Crate getCrate(int index);

        void yaBastaFreezer();

    private:
        cv::Rect floorRect;
        cv::Mat floorImage;
        std::vector< Crate > crates;
        cv::Mat crateMask;
        cv::Mat emptyMask;
        cv::Mat foregroundMask;
        cv::Mat wrongProductsMask;
        bool moveFlag;
        std::vector< cv::FileNode > productsNodes;
        std::vector< std::string > productsList;
        std::string dataPath;

}; 

class Shelf
{ 
    public:

        /*
         * Constructor de Shelf
         * @param shelfImage_ imagen de la repisa
         * @param emptyThreshold umbral para calcular vacios
         * @param jsonPath ruta del archivo .json
         * @param chargeFromJson bool que indica si cargar o no del .json
         * @param id id de la repisa
         */
        Shelf(  cv::Mat shelfImage_ = cv::Mat(), int emptyThreshold=160,
                std::string const jsonPath = "", bool chargeFromJson = false, int id = 0);

        /*
         * Carga informacion de la repisa desde el .json
         * @param jsonPath ruta al archivo .json
         * @param id id de la repisa
         */
        void loadShelfFromJson(std::string const  jsonPath, int id);

        /*
         * Carga productos segun el tipo de producto indicado en el .json
         * @param jsonPath ruta al archivo .json
         * @param id id de la repisa
         */
        void loadProductsFromJson(std::string const jsonPath, int id);

        /*
         * Guarda configuracion actual en un .json
         * No implementado
         */

        void saveJSON();

        /*
        * Actualiza la imagen del Shelf
        * @param frame_ imagen extraida del video
        * @param fg_ imagen del foreground
        */
        void updateShelf(cv::Mat frame_, cv::Mat fg_);

        /*
        * Actualiza la imagen del Shelf y de los pisos
        * @param frame imagen extraida del video
        */
        void updateImage(cv::Mat frame);

        /*
        * Setea la mascara de Foreground, en cada piso de la repisa
        * @param fgMask_ mascara de Foreground a setear
        */
        void setFgMask(cv::Mat fgMask_);
        // -----------------------------------------

        /*
        * Revisa si la mascara del Foreground esta 
        * interactuando con los pisos de la repisa
        */
        void checkIfFgIntersectsFloors();

        /*
        * Calcula informacion de la repisa, como thickness y centers de los pisos
        * @param image imagen de la repisa
        */
        void calcShelfInfo(cv::Mat image);

        /*
        * Calcula la mascara de los espacios vacios en la repisa
        */
        void calcEmptyMask();

        /*
        * Calcula la mascara de los productos incorrectos en la repisa
        */
        void calcWrongProductsMask();

        /*
        * Calcula los pisos, a traves de las medidas de Shelf
        * @param height altura de la repisa
        * @param width anchura de la repisa
        */
        void fillFloorsVect(int height, int width);

        /*
        * Pinta cada piso con distintos colores, esto para probar metodos
        * @param image imagen a pintar
        */
        void paintFloorRects(cv::Mat image);

        /*
        * Dibuja en la repisa los cuadrados de los espacios vacios y objetos incorrectos
        * @param drawEmpty bandera para dibujar los blobs de los espacios vacios
        * @param drawWrong bandera para dibujar los blobs de los objetos incorrectos
        */
        void drawGuiImage(bool drawEmpty, bool drawWrong);

        /*
        * Retorna un piso de la repisa en especifico
        * @param floor numero del piso de la repisa a utilizar
        * @return piso de la repisa en especifico
        */
        Floor getFloor(int floor);

        /*
        * Llama a calcShelfMask, para calcular la mascara de la repisa,
        * y retorna esta mascara
        * @param height altura de la mascara
        * @param width ancho de la mascara
        * @return matriz de la mascara 
        */
        cv::Mat getShelfMask(int height, int width);

        /*
        * @return matriz de imagen de la repisa
        */
        cv::Mat getShelfImage();

        /*
        * @return matriz de imagen de la mascara vacia
        */
        cv::Mat getEmptyMask();

        /*
        * @return matriz de la mascara de foreground
        */
        cv::Mat getFgMask();
        
        /*
        * @return matriz de la mascara con los productos incorrectos
        */
        cv::Mat getWrongProductMask();

        /*
        * @return matriz de imagen con la interfaz grafica del usuario
        */
        cv::Mat getGuiImage();

        /*
        * @return bool que define si hay algo moviendose (true) o no (false)
        */
        bool isMoving();
        
    private:
        int emptyThreshold;

        int thickness;
        std::vector<int> centers;
        std::vector<Floor> floors;
        
        std::vector< std::string > productsList;
        std::vector< cv::FileNode > productsNodes;
        cv::FileNode shelfNode;
        std::string dataPath;

        cv::Mat shelfMask;
        cv::Mat emptyMask;
        cv::Mat shelfImage;
        cv::Mat wrongProductMask;
        cv::Mat foregroundMask;
        cv::Mat guiImage;
        cv::Size imgSize;

        std::time_t last_movement_time;
        bool fgDidMove;

        void calcShelfMask(int height, int width);

        int avrg_row(std::vector<int> rows);
        
        int get_lines_thickness(cv::Mat img);
        
}; 

#endif