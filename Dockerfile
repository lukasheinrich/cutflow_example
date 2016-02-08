FROM lukasheinrich/atlas-analysisbase-2.4.3
RUN mkdir /analysis
COPY . /analysis
WORKDIR /analysis
RUN curl -s wget https://bootstrap.pypa.io/get-pip.py | python -
RUN pip install pyyaml
RUN bash -c '\
    source /atlas-asg/get_rcsetup.sh &&\
    rcSetup Base,2.4.3 &&\
    rc find_packages &&\
    rc compile'
   
    