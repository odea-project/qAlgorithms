import pandas as pd
import numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
import joblib
import matplotlib.pyplot as plt
import os

valid_regression_file = 'test/logfile_validRegression.txt'
invalid_regression_file = 'test/logfile_invalidRegression.txt'

# Load valid regression data
valid_regression_data = pd.read_csv(valid_regression_file, sep=' ', header=None, usecols=[1, 2, 3, 4])

# Load invalid regression data
invalid_regression_data = pd.read_csv(invalid_regression_file, sep=' ', header=None, usecols=[1, 2, 3, 4])

# filter inf and nan values from invalid regression data
invalid_regression_data = invalid_regression_data.replace([np.inf, -np.inf], np.nan).dropna()

print(valid_regression_data)
# Create the feature matrix X and the target vector y
X = pd.concat([valid_regression_data, invalid_regression_data])
y = pd.concat([pd.Series(np.ones(len(valid_regression_data))), pd.Series(np.zeros(len(invalid_regression_data)))])

# Split the data into training and testing sets
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Create a random forest classifier
rf_classifier = RandomForestClassifier(n_estimators = 16,verbose=3,n_jobs=-1)

# Check if the model file exists
if os.path.isfile('random_forest_model.pkl'):
  # Load the trained model from the file
  rf_classifier = joblib.load('random_forest_model.pkl')
else:
  # Train the classifier
  rf_classifier.fit(X_train, y_train)

# Predict the classes for the test set
y_pred = rf_classifier.predict(X_test)

# Calculate the accuracy of the model
accuracy = accuracy_score(y_test, y_pred)
# Get the feature importances
importances = rf_classifier.feature_importances_

# Print the feature importances
print("Feature Importances:")
for feature, importance in zip(X.columns, importances):
  print(f"{feature}: {importance}")
# Save the trained model to a file
joblib.dump(rf_classifier, 'random_forest_model.pkl')
print("Accuracy:", accuracy)


