import numpy as np
from PIL import Image
from scipy.stats import mode
from scipy.ndimage import binary_dilation, binary_erosion


def apply_morphological_operations(mask, kernel_size=5, iterations=2):
    kernel = np.ones((kernel_size, kernel_size), dtype=bool)

    for _ in range(iterations):
        mask = binary_erosion(mask, structure=kernel)

    for _ in range(iterations):
        mask = binary_dilation(mask, structure=kernel)

    return mask

def remove_background_by_sixteenths(image_path, tolerance=2):
    image_np = np.array(Image.open(image_path).convert("RGB"))
    height, width, _ = image_np.shape

    quarter_height = height // 4
    quarter_width = width // 4

    parts = []
    for i in range(4):
        for j in range(4):
            parts.append(image_np[i * quarter_height:(i + 1) * quarter_height, j * quarter_width:(j + 1) * quarter_width])

    parts_processed = []
    for i, part in enumerate(parts):
        reshaped = part.reshape(-1, 3)
        dominant_color = mode(reshaped, axis=0)[0]

        mask = np.all((reshaped >= (dominant_color - tolerance[i]))
          & (reshaped <= (dominant_color + tolerance[i])), axis=1)

        mask = apply_morphological_operations(mask.reshape(part.shape[:2])).reshape(-1)
        reshaped[mask] = [0, 255, 0]

        parts_processed.append(reshaped.reshape(part.shape))

    return Image.fromarray(np.vstack([np.hstack(parts_processed[i * 4:(i + 1) * 4]) for i in range(4)]))

image_path = "nRVTeIjvr11f.jpg"
result = remove_background_by_sixteenths(image_path, tolerance=[12, 22, 22, 34, 15, 30, 20, 50, 15, 25, 20, 25, 25, 55, 30, 30])

result.save("cats_without_dominant_color.png")
