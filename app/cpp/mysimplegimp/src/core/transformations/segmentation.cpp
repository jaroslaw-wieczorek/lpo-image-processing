#include "segmentation.h"

#include "conversion_grayscale.h"
#include "blur_gaussian.h"

#include <queue>

Segmentation::Segmentation(PNM* img) :
    Transformation(img)
{
}

Segmentation::Segmentation(PNM* img, ImageViewer* iv) :
    Transformation(img, iv)
{
}


QPoint* Segmentation::neighbourhood(QPoint p)
{

    return new QPoint();
}

PNM* Segmentation::transform()
{
    int width = image->width();
    int height = image->height();

    PNM* newImage = new PNM(width, height, QImage::Format_Grayscale8);

    ConversionGrayscale* conversionGrayscale = new ConversionGrayscale(image);
    PNM* grayImage = conversionGrayscale->transform();

    // matrix with labels
    math::matrix<int> lab (width, height);
    // matrix wiht distances
    math::matrix<int> dist (width, height);

    int current_label = 0;
    std::queue<QPoint> fifo;

    // Fill matrixes with -1 for lab and 0 for dist
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            lab[i][j] = -1;
            dist[i][j] = 0;
        }
    }

    // Create sorted list of gray pixels
    std::list<int> hSorted;
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            hSorted.push_back(qGray(grayImage->pixel(i, j)));
        }
    }

    // Sort and remove dupliactes of pixels values in queue
    hSorted.sort();
    hSorted.unique();

    // Set current distance on 0
    int current_distance = 0;

    // Iterate over queue of gray pixels
    for (std::list<int>::iterator h = hSorted.begin(); h != hSorted.end(); h++)
    {
        // Iterate over image
        for (int p_i = 0; p_i < width; p_i++)
        {
            for (int p_j = 0; p_j < height; p_j++)
            {
                // Check if the grey pixel from the image is the same as the one from the queue
                if (qGray(image->pixel(p_i, p_j)) == *h)
                {
                    // If true set lab in this place on -2
                    lab[p_i][p_j] = -2;

                    // Check neighbours -1 0 1 left-right and up-down for p_i, p_j
                    for (int x = -1; x < 2; x++)
                    {
                        for (int y = -1; y < 2; y++)
                        {
                            // Check if is not center
                            if (x != 0 || y != 0)
                            {
                                // Create q_i and q_j
                                int q_i = p_i+x;
                                int q_j = p_j+y;

                                // Check that the values(q_i, q_j) are within the image dimensions
                                if (q_i >= 0 && q_j >= 0 && q_i < width && q_j < height)
                                {
                                    // Check point value lab[q_i][q_j] is greater than or equal to zero
                                    if (lab[q_i][q_j] >= 0)
                                    {
                                        // Initialize queue with neighbours at level h
                                        // of current basins or watersheds
                                        dist[p_i][p_j] = 1;
                                        fifo.push(QPoint(p_i, p_j));
                                    } //end if - Check point value lab[q_i][q_j] is greater than or equal to zero
                                } // end if - Check that the values(q_i, q_j) are within the image dimensions
                            } // end if - Check if is not center
                        } //end for
                    } // end for - Check neighbours -1 0 1 left-right and up-down for p_i, p_j
                } // end if - Check if the grey pixel from the image is the same as the one from the queue
            } // end for
        } //end for - Iterate over image

        // Set current distance on 1
        current_distance = 1;
        fifo.push(QPoint(-1, -1));

        // while loop
        while (true)
        {
            // Get first element and remove from queue
            QPoint p = fifo.front();
            fifo.pop();

            // Check if x and y equal -1
            if (p.x() == -1 && p.y() == -1)
            {
                // Check ff queue is empty - break loop
                if (fifo.empty())
                {
                    break;
                }
                else
                {
                    // Push to queue Point(-1, -1)
                    fifo.push(QPoint(-1, -1));

                    // Increment current distanece
                    current_distance += 1;

                    // Get first element and remove from queue
                    p = fifo.front();
                    fifo.pop();
                }
            }

            // Check neighbours -1 0 1 left-right and up-down for p_i, p_j
            for (int x = -1; x < 2; x++)
            {
                for (int y = -1; y < 2; y++)
                {
                    // Check if is not center
                    if (x != 0 || y != 0)
                    {
                        // Get current point x, y values and create q_i, q_j
                        int p_i = p.x();
                        int p_j = p.y();
                        int q_i = p_i + x;
                        int q_j = p_j + y;

                        // Check that the values(q_i, q_j) are within the image dimensions
                        if (q_i >= 0 && q_j >= 0 && q_i < width && q_j < height)
                        {
                            // Check distance value of point q_i, q_j is less than current distance and lab[q_i][q_j] is greater than or equal to zero
                            if (dist[q_i][q_j] < current_distance && lab[q_i][q_j] >= 0)
                            {
                                // Check if lab point for q_i and q_j is grather than 0
                                if (lab[q_i][q_j] > 0)
                                {
                                    if (lab[p_i][p_j] == -2 or lab[p_i][p_j] == 0)
                                    {
                                        lab[p_i][p_j] = lab[q_i][q_j];
                                    }
                                    else if (lab[p_i][p_j] != lab[q_i][q_j])
                                    {
                                        lab[p_i][p_j] = 0;
                                    }
                                }
                                else if (lab[p_i][p_j] == -2)
                                {
                                    lab[p_i][p_j] = 0;
                                } // end else - Check if lab point for q_i and q_j is grather than 0

                            }
                            else if (dist[q_i][q_j] == 0 && lab[q_i][q_j] == -2)
                            {
                                // The q is plateau pixel
                                dist[q_i][q_j] = current_distance + 1;
                                fifo.push(QPoint(q_i, q_j));
                            }
                        } // end if - Check that the values(q_i, q_j) are within the image dimensions
                    } // end if (x != 0 || y != 0)
                } // end for
            } // end for - Check neighbours -1 0 1 left-right and up-down for p_i, p_j
        } // end while

        // Iterate over image - detect and process new minimal at level h
        for (int p_i = 0; p_i < width; p_i++)
        {
            for (int p_j = 0; p_j < height; p_j++)
            {
                // Check if the grey pixel from the image is the same as the one from the queue
                if (qGray(image->pixel(p_i, p_j)) == *h)
                {
                    // Set distance to zero
                    dist[p_i][p_j] = 0;

                    // Check if lab[p_i][p_j] is -2
                    if (lab[p_i][p_j] == -2)
                    {
                        // Increment current lab
                        current_label += 1;
                        // Push point[p_i, p_j] to queue
                        fifo.push(QPoint(p_i, p_j));
                        // Set current lab value to lab[p_i][p_j]
                        lab[p_i][p_j] = current_label;

                        // while loop until the queue isn't empty
                        while (!fifo.empty())
                        {
                            QPoint q = fifo.front();
                            fifo.pop();

                            // Check neighbours -1 0 1 left-right and up-down for q_i, q_j
                            for (int x = -1; x < 2; x++)
                            {
                                for (int y = -1; y < 2; y++)
                                {
                                    if (x != 0 || y != 0)
                                    {
                                        int q_i = q.x();
                                        int q_j = q.y();
                                        int r_i = q_i + x;
                                        int r_j = q_j + y;
                                        if (r_i >= 0 && r_j >= 0 && r_i < width && r_j < height)
                                        {
                                            if (lab[r_i][r_j] == -2)
                                            {
                                                fifo.push(QPoint(r_i, r_j));
                                                lab[r_i][r_j] = current_label;
                                            } // end if set lab[r_i][r_j] == -2

                                        } // end if (r_i >= 0 && r_j >= 0 && r_i < width && r_j < height)
                                    } // end if (x != 0 || y != 0)
                                } // end for inspect neighbours of q
                            } // end for - Check neighbours -1 0 1 left-right and up-down for q_i, q_j
                        } // end while - while loop until the queue isn't empty
                    } // end if - Check if lab[p_i][p_j] is -2
                } // end if - Check if the grey pixel from the image is the same as the one from the queue
            } // end for
        } // end for - Iterate over image - detect and process new minimal at level h
    } // end for - Iterate over queue of gray pixels

    // Set very small value
    int max = -100;

    // Get max for lab points
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if (lab[i][j] > max)
            {
                max = lab[i][j];
            }
        }
    }
    // Protection against division by 0
    if (max == 0)
    {
        max = 1;
    }

    // Draw image
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            double pixel = lab[i][j] * 255/max;
            double r = pixel * 0.3;
            double g = pixel * 0.6;
            double b = pixel * 0.1;
            QColor newPixel = QColor(r+g+b,r+g+b,r+g+b);
            newImage->setPixel(i, j, QColor(newPixel).rgb());
        }
    }
    return newImage;
}
