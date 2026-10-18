import streamlit as st
import tensorflow as tf
import numpy as np
import os
from PIL import Image
import io
from pathlib import Path

# Set page config
st.set_page_config(
    page_title="Plant Disease Detection",
    page_icon="🌿",
    layout="wide",
    initial_sidebar_state="expanded"
)

# Custom CSS for better styling
st.markdown("""
<style>
    .main-header {
        font-size: 3rem;
        color: #2E8B57;
        text-align: center;
        margin-bottom: 2rem;
    }
    .prediction-box {
        background-color: #1e293b;
        color: #f8fafc;
        padding: 1.25rem;
        border-radius: 12px;
        border-left: 5px solid #22c55e;
        margin: 1rem 0;
    }
    .prediction-box h3,
    .prediction-box p,
    .prediction-box strong {
        color: #f8fafc !important;
    }
    .confidence-bar {
        background-color: #1e293b;
        border: 1px solid #334155;
        border-radius: 10px;
        padding: 0.5rem;
        margin: 0.5rem 0 1rem 0;
    }
    .confidence-fill {
        background-color: #22c55e;
        height: 20px;
        border-radius: 8px;
        transition: width 0.5s ease;
    }
</style>
""", unsafe_allow_html=True)

# Class names for disease detection
CLASS_NAMES = [
    'Apple___Apple_scab',
    'Apple___Black_rot',
    'Apple___Cedar_apple_rust',
    'Apple___healthy',
    'Blueberry___healthy',
    'Cherry_(including_sour)___Powdery_mildew',
    'Cherry_(including_sour)___healthy',
    'Corn_(maize)___Cercospora_leaf_spot Gray_leaf_spot',
    'Corn_(maize)___Common_rust_',
    'Corn_(maize)___Northern_Leaf_Blight',
    'Corn_(maize)___healthy',
    'Grape___Black_rot',
    'Grape___Esca_(Black_Measles)',
    'Grape___Leaf_blight_(Isariopsis_Leaf_Spot)',
    'Grape___healthy',
    'Orange___Haunglongbing_(Citrus_greening)',
    'Peach___Bacterial_spot',
    'Peach___healthy',
    'Pepper,_bell___Bacterial_spot',
    'Pepper,_bell___healthy',
    'Potato___Early_blight',
    'Potato___Late_blight',
    'Potato___healthy',
    'Raspberry___healthy',
    'Soybean___healthy',
    'Squash___Powdery_mildew',
    'Strawberry___Leaf_scorch',
    'Strawberry___healthy',
    'Tomato___Bacterial_spot',
    'Tomato___Early_blight',
    'Tomato___Late_blight',
    'Tomato___Leaf_Mold',
    'Tomato___Septoria_leaf_spot',
    'Tomato___Spider_mites Two-spotted_spider_mite',
    'Tomato___Target_Spot',
    'Tomato___Tomato_Yellow_Leaf_Curl_Virus',
    'Tomato___Tomato_mosaic_virus',
    'Tomato___healthy'
]

# Disease information dictionary
DISEASE_INFO = {
    'Apple___Apple_scab': {
        'description': 'A fungal disease that causes dark, scabby lesions on leaves and fruit.',
        'treatment': 'Apply fungicides, remove infected plant debris, and ensure good air circulation.',
        'severity': 'Moderate'
    },
    'Apple___Black_rot': {
        'description': 'A fungal disease that causes black rot on fruit and cankers on branches.',
        'treatment': 'Prune infected branches, apply fungicides, and maintain tree health.',
        'severity': 'High'
    },
    'Apple___Cedar_apple_rust': {
        'description': 'A fungal disease that requires both apple trees and cedar trees to complete its life cycle.',
        'treatment': 'Remove nearby cedar trees if possible, apply fungicides during spring.',
        'severity': 'Moderate'
    },
    'Apple___healthy': {
        'description': 'Your apple tree appears to be healthy with no visible disease symptoms.',
        'treatment': 'Continue regular maintenance and monitoring.',
        'severity': 'None'
    },
    'Blueberry___healthy': {
        'description': 'Your blueberry plant appears to be healthy with no visible disease symptoms.',
        'treatment': 'Continue regular maintenance and monitoring.',
        'severity': 'None'
    },
    'Cherry_(including_sour)___Powdery_mildew': {
        'description': 'A fungal disease that creates white powdery spots on leaves and fruit.',
        'treatment': 'Apply fungicides, improve air circulation, and avoid overhead watering.',
        'severity': 'Moderate'
    },
    'Cherry_(including_sour)___healthy': {
        'description': 'Your cherry tree appears to be healthy with no visible disease symptoms.',
        'treatment': 'Continue regular maintenance and monitoring.',
        'severity': 'None'
    },
    'Corn_(maize)___Cercospora_leaf_spot Gray_leaf_spot': {
        'description': 'A fungal disease that causes gray to tan lesions on corn leaves.',
        'treatment': 'Apply fungicides, rotate crops, and remove infected plant debris.',
        'severity': 'High'
    },
    'Corn_(maize)___Common_rust_': {
        'description': 'A fungal disease that produces reddish-brown pustules on corn leaves.',
        'treatment': 'Apply fungicides, plant resistant varieties, and manage crop rotation.',
        'severity': 'Moderate'
    },
    'Corn_(maize)___Northern_Leaf_Blight': {
        'description': 'A fungal disease that causes cigar-shaped lesions on corn leaves.',
        'treatment': 'Apply fungicides, plant resistant varieties, and remove crop debris.',
        'severity': 'High'
    },
    'Corn_(maize)___healthy': {
        'description': 'Your corn plant appears to be healthy with no visible disease symptoms.',
        'treatment': 'Continue regular maintenance and monitoring.',
        'severity': 'None'
    },
    'Grape___Black_rot': {
        'description': 'A fungal disease that causes black rot on grape clusters and leaves.',
        'treatment': 'Apply fungicides, remove infected plant parts, and improve air circulation.',
        'severity': 'High'
    },
    'Grape___Esca_(Black_Measles)': {
        'description': 'A fungal disease that causes wood decay and leaf symptoms in grapevines.',
        'treatment': 'Prune infected wood, apply fungicides, and maintain vine health.',
        'severity': 'High'
    },
    'Grape___Leaf_blight_(Isariopsis_Leaf_Spot)': {
        'description': 'A fungal disease that causes brown spots with yellow halos on grape leaves.',
        'treatment': 'Apply fungicides, remove infected leaves, and improve air circulation.',
        'severity': 'Moderate'
    },
    'Grape___healthy': {
        'description': 'Your grapevine appears to be healthy with no visible disease symptoms.',
        'treatment': 'Continue regular maintenance and monitoring.',
        'severity': 'None'
    },
    'Orange___Haunglongbing_(Citrus_greening)': {
        'description': 'A bacterial disease spread by insects that causes yellowing and misshapen fruit.',
        'treatment': 'Remove infected trees, control insect vectors, and use disease-free planting material.',
        'severity': 'Very High'
    },
    'Peach___Bacterial_spot': {
        'description': 'A bacterial disease that causes spots on leaves and fruit of peach trees.',
        'treatment': 'Apply copper-based bactericides, prune infected parts, and improve air circulation.',
        'severity': 'Moderate'
    },
    'Peach___healthy': {
        'description': 'Your peach tree appears to be healthy with no visible disease symptoms.',
        'treatment': 'Continue regular maintenance and monitoring.',
        'severity': 'None'
    },
    'Pepper,_bell___Bacterial_spot': {
        'description': 'A bacterial disease that causes spots on pepper leaves and fruit.',
        'treatment': 'Apply copper-based bactericides, remove infected plants, and avoid overhead watering.',
        'severity': 'Moderate'
    },
    'Pepper,_bell___healthy': {
        'description': 'Your bell pepper plant appears to be healthy with no visible disease symptoms.',
        'treatment': 'Continue regular maintenance and monitoring.',
        'severity': 'None'
    },
    'Potato___Early_blight': {
        'description': 'A fungal disease that causes dark brown spots with concentric rings on potato leaves.',
        'treatment': 'Apply fungicides, remove infected leaves, and avoid overhead watering.',
        'severity': 'Moderate'
    },
    'Potato___Late_blight': {
        'description': 'A devastating fungal disease that can quickly destroy potato crops.',
        'treatment': 'Apply fungicides immediately, remove infected plants, and ensure good drainage.',
        'severity': 'Very High'
    },
    'Potato___healthy': {
        'description': 'Your potato plant appears to be healthy with no visible disease symptoms.',
        'treatment': 'Continue regular maintenance and monitoring.',
        'severity': 'None'
    },
    'Raspberry___healthy': {
        'description': 'Your raspberry plant appears to be healthy with no visible disease symptoms.',
        'treatment': 'Continue regular maintenance and monitoring.',
        'severity': 'None'
    },
    'Soybean___healthy': {
        'description': 'Your soybean plant appears to be healthy with no visible disease symptoms.',
        'treatment': 'Continue regular maintenance and monitoring.',
        'severity': 'None'
    },
    'Squash___Powdery_mildew': {
        'description': 'A fungal disease that creates white powdery spots on squash leaves.',
        'treatment': 'Apply fungicides, improve air circulation, and avoid overhead watering.',
        'severity': 'Moderate'
    },
    'Strawberry___Leaf_scorch': {
        'description': 'A fungal disease that causes brown spots and scorched appearance on strawberry leaves.',
        'treatment': 'Apply fungicides, remove infected leaves, and improve air circulation.',
        'severity': 'Moderate'
    },
    'Strawberry___healthy': {
        'description': 'Your strawberry plant appears to be healthy with no visible disease symptoms.',
        'treatment': 'Continue regular maintenance and monitoring.',
        'severity': 'None'
    },
    'Tomato___Bacterial_spot': {
        'description': 'A bacterial disease that causes spots on tomato leaves and fruit.',
        'treatment': 'Apply copper-based bactericides, remove infected plants, and avoid overhead watering.',
        'severity': 'Moderate'
    },
    'Tomato___Early_blight': {
        'description': 'A fungal disease that causes dark brown spots with concentric rings on tomato leaves.',
        'treatment': 'Apply fungicides, remove infected leaves, and avoid overhead watering.',
        'severity': 'Moderate'
    },
    'Tomato___Late_blight': {
        'description': 'A devastating fungal disease that can quickly destroy tomato crops.',
        'treatment': 'Apply fungicides immediately, remove infected plants, and ensure good drainage.',
        'severity': 'Very High'
    },
    'Tomato___Leaf_Mold': {
        'description': 'A fungal disease that causes yellow spots on tomato leaves with gray mold underneath.',
        'treatment': 'Apply fungicides, improve air circulation, and reduce humidity.',
        'severity': 'Moderate'
    },
    'Tomato___Septoria_leaf_spot': {
        'description': 'A fungal disease that causes small, circular spots with gray centers on tomato leaves.',
        'treatment': 'Apply fungicides, remove infected leaves, and avoid overhead watering.',
        'severity': 'Moderate'
    },
    'Tomato___Spider_mites Two-spotted_spider_mite': {
        'description': 'Tiny arachnids that feed on tomato plants, causing stippling and webbing.',
        'treatment': 'Apply miticides, introduce beneficial insects, and maintain plant health.',
        'severity': 'Moderate'
    },
    'Tomato___Target_Spot': {
        'description': 'A fungal disease that causes target-like spots on tomato leaves and fruit.',
        'treatment': 'Apply fungicides, remove infected leaves, and improve air circulation.',
        'severity': 'Moderate'
    },
    'Tomato___Tomato_Yellow_Leaf_Curl_Virus': {
        'description': 'A viral disease spread by whiteflies that causes yellowing and curling of tomato leaves.',
        'treatment': 'Control whiteflies, remove infected plants, and use virus-resistant varieties.',
        'severity': 'High'
    },
    'Tomato___Tomato_mosaic_virus': {
        'description': 'A viral disease that causes mosaic patterns and distortion on tomato leaves.',
        'treatment': 'Remove infected plants, control aphids, and use virus-free seed.',
        'severity': 'High'
    },
    'Tomato___healthy': {
        'description': 'Your tomato plant appears to be healthy with no visible disease symptoms.',
        'treatment': 'Continue regular maintenance and monitoring.',
        'severity': 'None'
    }
}

@st.cache_resource
def load_model():
    """Load the trained model with caching"""
    try:
        # Resolve the directory containing this script to look for the model files there
        base_dir = Path(__file__).resolve().parent
        model_path_h5 = base_dir / "trained_model.h5"
        model_path_keras = base_dir / "trained_model.keras"

        # Prefer the HDF5 model when both are present (better cross-version compatibility)
        if model_path_h5.exists():
            model = tf.keras.models.load_model(model_path_h5)
            st.success(f"✅ Model loaded successfully from {model_path_h5.name}")
            return model
        elif model_path_keras.exists():
            model = tf.keras.models.load_model(model_path_keras)
            st.success(f"✅ Model loaded successfully from {model_path_keras.name}")
            return model
        else:
            st.error("❌ No model file found in the application directory. Please ensure 'trained_model.h5' or 'trained_model.keras' exists next to main.py.")
            return None
    except Exception as e:
        st.error(f"❌ Error loading model: {str(e)}")
        st.info("💡 Verify that the model file is compatible with your installed TensorFlow version.")
        return None

def preprocess_image(image_file):
    """Preprocess uploaded image for prediction"""
    try:
        # Load and resize image
        image = Image.open(image_file)
        image = image.resize((128, 128))
        
        # Convert to array and normalize
        image_array = tf.keras.preprocessing.image.img_to_array(image)
        image_array = np.expand_dims(image_array, axis=0)
        
        return image_array, image
    except Exception as e:
        st.error(f"Error preprocessing image: {str(e)}")
        return None, None

def predict_disease(image_file):
    """Predict disease from uploaded image"""
    try:
        model = load_model()
        if model is None:
            return None, None, None
        
        # Preprocess image
        image_array, original_image = preprocess_image(image_file)
        if image_array is None:
            return None, None, None
        
        # Make prediction
        predictions = model.predict(image_array)
        predicted_class_index = np.argmax(predictions[0])
        confidence_score = float(predictions[0][predicted_class_index])
        
        return predicted_class_index, confidence_score, original_image
        
    except Exception as e:
        st.error(f"Error during prediction: {str(e)}")
        return None, None, None

def display_prediction_result(class_index, confidence_score, original_image):
    """Display prediction results with detailed information"""
    if class_index is None:
        return
    
    predicted_class = CLASS_NAMES[class_index]
    disease_info = DISEASE_INFO.get(predicted_class, {})
    
    # Create columns for layout
    col1, col2 = st.columns([1, 1])
    
    with col1:
        st.subheader("🔍 Analysis Results")
        
        # Display prediction with confidence
        st.markdown(f"""
        <div class="prediction-box">
            <h3>Predicted Disease: {predicted_class.replace('___', ' - ')}</h3>
            <p><strong>Confidence:</strong> {confidence_score:.2%}</p>
        </div>
        """, unsafe_allow_html=True)
        
        # Confidence bar
        st.markdown(f"""
        <div class="confidence-bar">
            <div class="confidence-fill" style="width: {confidence_score * 100}%"></div>
        </div>
        """, unsafe_allow_html=True)
        
        # Disease information
        if disease_info:
            st.subheader("📋 Disease Information")
            st.write(f"**Description:** {disease_info.get('description', 'No description available.')}")
            st.write(f"**Severity:** {disease_info.get('severity', 'Unknown')}")
            
            st.subheader("💡 Treatment Recommendations")
            st.write(disease_info.get('treatment', 'No treatment information available.'))
    
    with col2:
        st.subheader("🖼️ Uploaded Image")
        st.image(original_image, caption="Your uploaded image", use_container_width=True)

# Sidebar
st.sidebar.title("🌿 Plant Disease Detection")
st.sidebar.markdown("---")

app_mode = st.sidebar.selectbox(
    "Select Page",
    ["Home", "Disease Detection", "About"]
)

# Home Page
if app_mode == "Home":
    st.markdown('<h1 class="main-header">🌿 Plant Disease Detection System</h1>', unsafe_allow_html=True)
    
    # Display home page image if available
    if os.path.exists("home_page.jpeg"):
        st.image("home_page.jpeg", use_container_width=True)
    
    st.markdown("""
    ## Welcome to the Plant Disease Recognition System! 🌿🔍
    
    Our mission is to help you identify plant diseases efficiently using advanced machine learning technology. 
    Upload an image of a plant, and our system will analyze it to detect any signs of diseases.
    
    ### 🌟 How It Works
    1. **Upload Image:** Go to the **Disease Detection** page and upload a clear image of a plant with suspected diseases.
    2. **AI Analysis:** Our advanced machine learning model will process the image to identify potential diseases.
    3. **Detailed Results:** Get comprehensive results including disease identification, confidence level, and treatment recommendations.
    
    ### 🎯 Supported Plants
    Our system can detect diseases in various plants including:
    - **Fruits:** Apple, Cherry, Peach, Orange, Grape, Blueberry, Raspberry, Strawberry
    - **Vegetables:** Tomato, Potato, Bell Pepper, Corn, Squash
    - **Legumes:** Soybean
    
    ### 💡 Why Choose Our System?
    - **High Accuracy:** Utilizes state-of-the-art deep learning techniques
    - **User-Friendly:** Simple and intuitive interface
    - **Fast Results:** Get results in seconds
    - **Comprehensive:** Detailed disease information and treatment recommendations
    
    ### 🚀 Get Started
    Click on **Disease Detection** in the sidebar to upload an image and experience the power of our Plant Disease Recognition System!
    """)

# Disease Detection Page
elif app_mode == "Disease Detection":
    st.markdown('<h1 class="main-header">🔍 Disease Detection</h1>', unsafe_allow_html=True)
    
    st.markdown("""
    ### 📸 Upload Your Plant Image
    
    Please upload a clear, well-lit image of the plant you want to analyze. 
    For best results, ensure the image shows the leaves clearly and is taken in good lighting conditions.
    """)
    
    # File uploader
    uploaded_file = st.file_uploader(
        "Choose an image file",
        type=['jpg', 'jpeg', 'png', 'bmp'],
        help="Upload an image of a plant to detect diseases"
    )
    
    if uploaded_file is not None:
        # Display uploaded image
        st.subheader("📷 Your Uploaded Image")
        image = Image.open(uploaded_file)
        st.image(image, caption="Uploaded Image", use_container_width=True)
        
        # Prediction button
        if st.button("🔍 Analyze Image", type="primary", use_container_width=True):
            with st.spinner("🤖 AI is analyzing your image..."):
                # Make prediction
                predicted_class, confidence, original_img = predict_disease(uploaded_file)
                
                if predicted_class is not None:
                    st.success("✅ Analysis Complete!")
                    display_prediction_result(predicted_class, confidence, original_img)
                else:
                    st.error("❌ Analysis failed. Please try again with a different image.")

# About Page
elif app_mode == "About":
    st.markdown('<h1 class="main-header">ℹ️ About</h1>', unsafe_allow_html=True)
    
    st.markdown("""
    ## 📊 About the Dataset
    
    This dataset is recreated using offline augmentation from the original dataset. The original dataset can be found on GitHub repositories dedicated to plant disease detection.
    
    ### 📈 Dataset Statistics
    - **Total Images:** 87,000+ RGB images
    - **Categories:** 38 different classes (healthy and diseased)
    - **Plant Types:** 14 different plant species
    - **Training Set:** 70,295 images (80%)
    - **Validation Set:** 17,572 images (20%)
    - **Test Set:** 33 images for prediction
    
    ### 🌱 Supported Plant Categories
    1. **Apple** - Apple scab, Black rot, Cedar apple rust, Healthy
    2. **Blueberry** - Healthy
    3. **Cherry** - Powdery mildew, Healthy
    4. **Corn (Maize)** - Cercospora leaf spot, Common rust, Northern leaf blight, Healthy
    5. **Grape** - Black rot, Esca, Leaf blight, Healthy
    6. **Orange** - Huanglongbing (Citrus greening)
    7. **Peach** - Bacterial spot, Healthy
    8. **Bell Pepper** - Bacterial spot, Healthy
    9. **Potato** - Early blight, Late blight, Healthy
    10. **Raspberry** - Healthy
    11. **Soybean** - Healthy
    12. **Squash** - Powdery mildew
    13. **Strawberry** - Leaf scorch, Healthy
    14. **Tomato** - Bacterial spot, Early blight, Late blight, Leaf mold, Septoria leaf spot, Spider mites, Target spot, Yellow leaf curl virus, Mosaic virus, Healthy
    
    ### 🛠️ Technical Details
    - **Model:** Convolutional Neural Network (CNN)
    - **Framework:** TensorFlow/Keras
    - **Image Size:** 128x128 pixels
    - **Augmentation:** Rotation, flipping, brightness adjustment
    
    ### 🎯 Model Performance
    The model has been trained to achieve high accuracy in detecting various plant diseases, helping farmers and gardeners identify issues early for better crop management.
    """)

# Footer
st.markdown("---")
st.markdown("""
<div style='text-align: center; color: #666;'>
    <p>🌿 Plant Disease Detection System | Built with Streamlit and TensorFlow</p>
</div>
""", unsafe_allow_html=True)