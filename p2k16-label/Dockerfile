FROM python:3.9-slim-bullseye
RUN useradd --create-home --shell /bin/bash app_user
WORKDIR /home/app_user
COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt
USER app_user
COPY --chown=app_user . .
CMD ["python", "print_label_service.py"]
