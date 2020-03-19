FROM rhel7:latest
MAINTAINER Cun Gong <gong_cun@bocmacau.com>
LABEL description="puzzle container"

RUN yum install -y socat && yum clean all

ADD root /
EXPOSE 3001
CMD ["/puzzle-entry.sh"]

