FROM python:3.9-slim-bullseye
ADD . /code
WORKDIR /code
RUN pip install -r requirements.txt
CMD ["python", "print_label_service.py"]
