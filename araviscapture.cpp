#include "araviscapture.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

AravisCapture::AravisCapture() {
    if (ARV_IS_CAMERA(camera)) {
        std::cout << "Pixel format: " << arv_camera_get_pixel_format_as_string (camera) << std::endl;
        std::cout << "Available formats: " << std::endl;
        guint num = 0;
        const char ** formats = arv_camera_get_available_pixel_formats_as_strings(camera, &num);
        for (guint ii = 0; ii < num; ++ii) {
            std::cout << formats[ii] << ", ";
        }
        std::cout << std::endl;

        arv_camera_set_acquisition_mode(camera, ArvAcquisitionMode::ARV_ACQUISITION_MODE_SINGLE_FRAME);
    }
    else {
        std::cerr << "Camera initialization has failed." << std::endl;
    }
}

const char *  named_payload_type(ArvBufferPayloadType const x) {
    switch (x) {
    case ARV_BUFFER_PAYLOAD_TYPE_UNKNOWN: return "UNKNOWN";
    case ARV_BUFFER_PAYLOAD_TYPE_IMAGE: return "IMAGE";
    case ARV_BUFFER_PAYLOAD_TYPE_RAWDATA: return "RAWDATA";
    case ARV_BUFFER_PAYLOAD_TYPE_FILE: return "FILE";
    case ARV_BUFFER_PAYLOAD_TYPE_CHUNK_DATA: return "CHUNK_DATA";
    case ARV_BUFFER_PAYLOAD_TYPE_EXTENDED_CHUNK_DATA: return "EXTENDED_CHUNK_DATA";
    case ARV_BUFFER_PAYLOAD_TYPE_JPEG: return "JPEG";
    case ARV_BUFFER_PAYLOAD_TYPE_JPEG2000: return "JPEG2000";
    case ARV_BUFFER_PAYLOAD_TYPE_H264: return "H264";
    case ARV_BUFFER_PAYLOAD_TYPE_MULTIZONE_IMAGE: return "MULTIZONE_IMAGE";
    }
    return "UNKNOWN";
}

const char * named_buffer_status (ArvBufferStatus const x) {
    switch (x) {
    case ARV_BUFFER_STATUS_UNKNOWN: return "UNKNOWN";
    case ARV_BUFFER_STATUS_SUCCESS: return "SUCCESS";
    case ARV_BUFFER_STATUS_CLEARED: return "CLEARED";
    case ARV_BUFFER_STATUS_TIMEOUT: return "TIMEOUT";
    case ARV_BUFFER_STATUS_MISSING_PACKETS: return "MISSING_PACKETS";
    case ARV_BUFFER_STATUS_WRONG_PACKET_ID: return "WRONG_PACKET_ID";
    case ARV_BUFFER_STATUS_SIZE_MISMATCH: return "SIZE_MISMATCH";
    case ARV_BUFFER_STATUS_FILLING:  return "FILLING";
    case ARV_BUFFER_STATUS_ABORTED: return "ABORTED";
    }
    return "UNKNOWN";
}


cv::Mat AravisCapture::capture() {
    if (!ARV_IS_CAMERA(camera)) {
        throw std::runtime_error("Camera is not valid");
    }
    arv_camera_set_pixel_format(camera, ARV_PIXEL_FORMAT_MONO_12);
    ArvBuffer * buffer = arv_camera_acquisition(camera, 0);
    gint const width = arv_buffer_get_image_width(buffer);
    gint const height = arv_buffer_get_image_height(buffer);

    std::cout << "w x h: " << width << "x" << height << std::endl;

    ArvPixelFormat format = arv_buffer_get_image_pixel_format(buffer);

    int bit_per_px = ARV_PIXEL_FORMAT_BIT_PER_PIXEL(format);

    ArvBufferPayloadType const payload = arv_buffer_get_payload_type(buffer);
    ArvBufferStatus const status = arv_buffer_get_status(buffer);
    std::cout << bit_per_px << " bit per px" << std::endl
              << "payload type: " << named_payload_type(payload) << std::endl
              << "status: " << named_buffer_status(status) << std::endl;

    uint16_t const * data = static_cast<uint16_t const * >(arv_buffer_get_data(buffer, nullptr));

    cv::Mat_<uint16_t> mat(height, width, uint16_t(0));

    for (int ii = 0; ii < height; ++ii) {
        for (int jj = 0; jj < width; ++jj) {
            mat(ii, jj) = data[ii * width + jj]*16;
        }
    }
    double minval = 0, maxval = 0;
    cv::minMaxIdx(mat, &minval, &maxval);
    std::cout << "min/max: " << minval << " / " << maxval << std::endl;
    std::cout << "exposure time: " << arv_camera_get_exposure_time(camera) << std::endl;
    //cv::imshow("captured", mat);
    //cv::waitKey(1);
    return std::move(mat);
}

int AravisCapture::hasCamera() {
    if (!ARV_IS_CAMERA(camera)) {
        return false;
    }
    arv_update_device_list();
    return int(arv_get_n_devices());
}

void AravisCapture::increaseExposureTime() {
    arv_camera_set_exposure_time(camera, 1.2 * arv_camera_get_exposure_time(camera));
}

void AravisCapture::decreaseExposureTime() {
    arv_camera_set_exposure_time(camera, 0.8 * arv_camera_get_exposure_time(camera));
}


void AravisCapture::loopTest() {

    while (true) {

        arv_update_device_list();

        unsigned int const n = arv_get_n_devices();
        std::cout << "Number of devices: " << n << std::endl;

        for (unsigned int ii = 0; ii < n; ++ii) {
            std::cout << "Info for device #" << ii << ":" << std::endl
                      << "device id:" << arv_get_device_id(ii) << std::endl
                      << "physical id: " << arv_get_device_physical_id(ii) << std::endl
                      << "model: " << arv_get_device_model(ii) << std::endl
                      << "serial: " << arv_get_device_serial_nbr(ii) << std::endl
                      << "vendor: " << arv_get_device_vendor(ii) << std::endl
                      << "address: " << arv_get_device_address(ii) << std::endl;

            std::cout << std::endl;
        }

        sleep(1);
    }
}
